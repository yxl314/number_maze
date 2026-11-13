#!/usr/bin/env python3
"""Enforce the number-maze layer contract.

Project includes must use an explicit layer prefix, e.g.
    #include "domain/graph.h"
    #include "view/board_view.h"

The checker rejects forbidden layer includes, traversal/absolute paths,
Qt headers in pure layers, Chinese display text in domain/model/service,
unlisted sources and manifest groups that point at the wrong layer.
"""

from __future__ import annotations

import re
import sys
from pathlib import Path, PurePosixPath

LAYERS = ("domain", "model", "service", "view", "controller", "app")

ALLOWED = {
    "domain": {"domain"},
    "model": {"domain", "model"},
    "service": {"domain", "service"},
    "view": {"domain", "view"},
    "controller": {"domain", "model", "service", "view", "controller"},
    "app": set(LAYERS),
}

QT_FORBIDDEN = {
    "model": {
        "QFile", "QSettings", "QFileDialog", "QMessageBox", "QWidget", "QDialog",
        "QMainWindow", "QStatusBar", "QMenu", "QToolBar", "QAction",
    },
    "service": {
        "QWidget", "QDialog", "QMessageBox", "QFileDialog", "QMainWindow",
        "QApplication", "QStatusBar", "QMenu", "QToolBar", "QAction",
    },
}

INCLUDE_RE = re.compile(r'^\s*#\s*include\s*([<"])([^>"]+)[>"]')
QT_INCLUDE_RE = re.compile(r'^\s*#\s*include\s*[<"]Q[A-Za-z0-9_/]*[>"]')
CJK_RE = re.compile(r'[\u4e00-\u9fff]')

# 只检查字符串字面量里的中文：那是会显示给用户的文本。注释里的中文是
# Doxygen 文档，不算显示文本，不能因此让整个 domain/model/service 全红。
STRING_LITERAL_RE = re.compile(r'"(?:[^"\\\n]|\\.)*"|\'(?:[^\'\\\n]|\\.)*\'')
MANIFEST_GROUP_RE = re.compile(
    r'^(NUMBERMAZE_(?:DOMAIN|MODEL|SERVICE|VIEW|CONTROLLER|APP|MAIN))'
    r'_(?:SOURCES|HEADERS)\s*=\s*(.*)$')
MANIFEST_GROUP_LAYER = {
    "NUMBERMAZE_DOMAIN": "domain",
    "NUMBERMAZE_MODEL": "model",
    "NUMBERMAZE_SERVICE": "service",
    "NUMBERMAZE_VIEW": "view",
    "NUMBERMAZE_CONTROLLER": "controller",
    "NUMBERMAZE_APP": "app",
    "NUMBERMAZE_MAIN": "app",
}


def layer_of(path: Path, root: Path) -> str | None:
    """Return the layer directory for a source file, or None."""
    try:
        rel = path.relative_to(root)
    except ValueError:
        return None
    if len(rel.parts) < 2:
        return None
    layer = rel.parts[0]
    return layer if layer in LAYERS else None


def normalize_include(target: str) -> str | None:
    """Return the canonical layer-prefixed include, or None for system headers."""
    target = target.replace("\\", "/")
    if target.startswith("/"):
        return ""
    if target.startswith("src/"):
        return ""
    parts = PurePosixPath(target).parts
    if ".." in parts:
        return ""
    if not parts or parts[0] not in LAYERS:
        return None
    return target


def logical_lines(text: str):
    """Yield manifest lines with backslash continuations joined."""
    parts: list[str] = []
    for raw in text.splitlines():
        line = raw.split("#", 1)[0].rstrip()
        if not line.strip():
            continue
        if line.endswith("\\"):
            parts.append(line[:-1].strip())
        else:
            parts.append(line.strip())
            yield " ".join(parts)
            parts = []
    if parts:
        yield " ".join(parts)


def check_manifest(root: Path, sources: list[Path]) -> list[str]:
    """Check that every source is listed and every manifest group is consistent."""
    problems: list[str] = []
    manifest = root / "number-maze.pri"
    if not manifest.is_file():
        return ["number-maze.pri is missing"]
    text = manifest.read_text(encoding="utf-8", errors="replace")

    # 按空白切成路径 token，避免 a.cpp 被 a.cpp.bak 之类的子串误命中。
    manifest_paths: set[str] = set()
    for line in logical_lines(text):
        for token in line.split():
            token = token.replace("$$NUMBERMAZE_ROOT/", "")
            manifest_paths.add(token)

    for source in sources:
        rel = source.relative_to(root).as_posix()
        if rel not in manifest_paths:
            problems.append(f"{source}: source file is not listed in number-maze.pri")

    listed: dict[str, list[str]] = {}
    for line in logical_lines(text):
        match = MANIFEST_GROUP_RE.match(line)
        if not match:
            continue
        group, values = match.group(1), match.group(2)
        expected = MANIFEST_GROUP_LAYER[group]
        for token in values.split():
            if not token.startswith("$$NUMBERMAZE_ROOT/"):
                continue
            rel = token[len("$$NUMBERMAZE_ROOT/"):]
            if not rel.startswith("src/"):
                continue
            parts = rel.split("/")
            if len(parts) < 3:
                continue
            actual = parts[1]
            if actual != expected:
                problems.append(
                    f"{manifest}: {group}_* lists {rel}, expected layer {expected}")
            listed.setdefault(rel, []).append(group)
            if not (root / rel).is_file():
                problems.append(f"{manifest}: listed file does not exist: {rel}")

    for rel, groups in listed.items():
        unique = sorted(set(groups))
        if len(unique) > 1:
            problems.append(
                f"{manifest}: {rel} is listed in multiple layer groups: {', '.join(unique)}")
    return problems


def main() -> int:
    root = Path(sys.argv[1] if len(sys.argv) > 1 else ".").resolve()
    src = root / "src"
    if not src.is_dir():
        print(f"layering: source directory not found: {src}")
        return 2

    problems: list[str] = []
    checked = 0
    sources: list[Path] = []

    for path in sorted(src.rglob("*")):
        if path.suffix not in {".h", ".hpp", ".cpp", ".cc", ".cxx"}:
            continue
        layer = layer_of(path, src)
        if layer is None:
            problems.append(f"{path}: file is not inside a known layer directory")
            continue
        checked += 1
        sources.append(path)
        text = path.read_text(encoding="utf-8", errors="replace")

        if layer in {"domain", "model", "service"}:
            for literal in STRING_LITERAL_RE.finditer(text):
                if CJK_RE.search(literal.group(0)):
                    lineno = text.count("\n", 0, literal.start()) + 1
                    problems.append(
                        f"{path}:{lineno}: Chinese display text is forbidden in {layer}/")
                    break

        for lineno, line in enumerate(text.splitlines(), 1):
            if layer == "domain" and QT_INCLUDE_RE.match(line):
                problems.append(f"{path}:{lineno}: domain must not include Qt headers")
                continue
            match = INCLUDE_RE.match(line)
            if not match:
                continue
            raw_target = match.group(2)
            canonical = normalize_include(raw_target)
            if canonical == "":
                problems.append(
                    f"{path}:{lineno}: non-canonical or escaping include is forbidden: {raw_target}")
                continue
            if canonical is None:
                # <QtCore/QSettings> 的模块前缀是 QtCore，真正头名是最后一段。
                header = raw_target.replace("\\", "/").rsplit("/", 1)[-1].split(".")[0]
                if layer in QT_FORBIDDEN and header in QT_FORBIDDEN[layer]:
                    problems.append(f"{path}:{lineno}: {layer} may not include {raw_target}")
                continue
            target_layer = canonical.split("/", 1)[0]
            if target_layer not in ALLOWED[layer]:
                problems.append(
                    f"{path}:{lineno}: {layer} may not include {target_layer} ({canonical})")

    problems.extend(check_manifest(root, sources))

    if problems:
        print("Architecture violations:")
        for problem in problems:
            print(f"  - {problem}")
        print(f"\n{len(problems)} violation(s) in {checked} files.")
        return 1

    print(f"Layering OK: {checked} files checked.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

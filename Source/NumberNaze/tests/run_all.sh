#!/bin/sh
# 构建并运行全部测试 —— 每个测试是独立工程，各自 qmake + make。
#
#     ./run_all.sh
#
# 不用 TEMPLATE=subdirs：qmake 的 subdirs 会按子目录名拼 .pro 路径，而本仓库
# 路径里有非 ASCII 目录（桌面），生成的 Makefile 会把路径写坏成 ??。
set -u

HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/../../.." && pwd)
QMAKE=${QMAKE:-qmake}
OUT="$ROOT/build/tests"
STATUS=0

for pro in "$HERE"/*_test.pro; do
    name=$(basename "$pro" .pro)
    dir="$OUT/$name"
    printf '=== %s ===\n' "$name"
    mkdir -p "$dir" || exit 1
    if ! (cd "$dir" && "$QMAKE" "$pro" && make -j"$(nproc)"); then
        printf '构建失败: %s\n' "$name"
        STATUS=1
        continue
    fi
    if ! "$dir/bin/$name"; then
        printf '测试失败: %s\n' "$name"
        STATUS=1
    fi
done

exit $STATUS

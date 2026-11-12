#!/bin/sh
# 构建并运行全部测试 —— 每个测试是独立 qmake 工程。
#
#     ./tests/run_all.sh            # 全部
#     ./tests/run_all.sh model      # 只跑名字含 model 的
#
# 产物放在 build/tests/；并行上限固定为 4。
set -u

HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/.." && pwd)
if [ -z "${QMAKE:-}" ]; then
    if [ -x /usr/lib/qt5/bin/qmake ]; then
        QMAKE=/usr/lib/qt5/bin/qmake
    else
        QMAKE=qmake
    fi
fi
JOBS=${JOBS:-4}
FILTER=${1:-}
OUT="$ROOT/build/tests"
STATUS=0

export QT_QPA_PLATFORM=offscreen

for pro in "$HERE"/*_tests.pro; do
    name=$(basename "$pro" .pro)
    case "$name" in
        *"$FILTER"*) ;;
        *) continue ;;
    esac
    dir="$OUT/$name"
    printf '=== %s ===\n' "$name"
    mkdir -p "$dir" || exit 1
    if ! (cd "$dir" && "$QMAKE" "$pro" && make -j"$JOBS"); then
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

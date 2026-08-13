#!/usr/bin/env bash

# Build, test, install, and consume libsl3 as a Linux shared object.
#
# Run from the repository root with ./tools/testso.sh. By default the script uses
# the Release configuration, fetched dependencies, and internal SQLite. Build
# directories are deleted before configuration.
#
# The script builds the shared object, runs the in-tree CTest suite against it,
# installs the package, builds tests/pkg-consumer against the install, and runs
# that consumer test.
#
# Examples:
#   ./tools/testso.sh --keep-build
#   ./tools/testso.sh --config Debug
#   ./tools/testso.sh --config RelWithDebInfo

set -euo pipefail

config="Release"
keep_build=false

usage() {
    cat <<'EOF'
Usage: ./tools/testso.sh [--config CONFIG] [--keep-build]

Build, test, install, and consume libsl3 as a Linux shared library.
EOF
}

while (($#)); do
    case "$1" in
        --config)
            config=${2:?missing value for --config}
            shift 2
            ;;
        --keep-build)
            keep_build=true
            shift
            ;;
        --help|-h)
            usage
            exit 0
            ;;
        *)
            echo "Unknown argument: $1" >&2
            usage >&2
            exit 2
            ;;
    esac
done

case "$config" in
    Debug|Release|RelWithDebInfo|MinSizeRel) ;;
    *)
        echo "Unsupported configuration: $config" >&2
        exit 2
        ;;
esac

repo_root=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
build_root="$repo_root/build/Linux"
build_dir="$build_root/ninja"
install_prefix="$build_dir/inst"
consumer_build_dir="$build_root/pkg-consumer-ninja-$config"

run() {
    printf '\n> '
    printf '%q ' "$@"
    printf '\n'
    "$@"
}

remove_build_directory() {
    local path=$1
    local resolved_path
    local resolved_root

    resolved_path=$(realpath -m -- "$path")
    resolved_root=$(realpath -m -- "$build_root")
    case "$resolved_path" in
        "$resolved_root"/*) ;;
        *)
            echo "Refusing to remove path outside build root: $resolved_path" >&2
            exit 1
            ;;
    esac

    if [[ -e "$resolved_path" ]]; then
        run rm -rf -- "$resolved_path"
    fi
}

if [[ "$keep_build" == false ]]; then
    remove_build_directory "$build_dir"
    remove_build_directory "$consumer_build_dir"
fi

run cmake --preset ninja \
    -DTOOLCHAIN_INCLUDES=toolchain/fetch-dependencies \
    -DBUILD_SHARED_LIBS=ON \
    -Dsl3_BUILD_TESTING=ON \
    -Dsl3_USE_INTERNAL_SQLITE3=ON
run cmake --build "$build_dir" --config "$config" --parallel
run ctest --test-dir "$build_dir" -C "$config" --output-on-failure

run cmake --install "$build_dir" \
    --config "$config" \
    --prefix "$install_prefix"

run cmake \
    -S "$repo_root/tests/pkg-consumer" \
    -B "$consumer_build_dir" \
    -G Ninja \
    -DCMAKE_BUILD_TYPE="$config" \
    -DCMAKE_PREFIX_PATH="$install_prefix"
run cmake --build "$consumer_build_dir" --parallel
run ctest --test-dir "$consumer_build_dir" --output-on-failure

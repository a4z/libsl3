#!/usr/bin/env bash

# Build, test, install, and consume libsl3 as a macOS dynamic library.
#
# Run from the repository root with ./testdynlib.sh. By default the script
# uses the Release configuration, fetched dependencies, and internal SQLite.
# Build directories are deleted before configuration.
#
# The script builds the dylib, runs the in-tree CTest suite against it,
# installs the package, verifies the installed dylib, builds
# tests/pkg-consumer against the install, and runs that consumer test.
#
# Examples:
#   ./testdynlib.sh --keep-build
#   ./testdynlib.sh --config Debug
#   ./testdynlib.sh --config RelWithDebInfo

set -euo pipefail

config="Release"
keep_build=false

usage() {
    cat <<'EOF'
Usage: ./testdynlib.sh [--config CONFIG] [--keep-build]

Build, test, install, and consume libsl3 as a macOS dynamic library.
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

if [[ $(uname -s) != Darwin ]]; then
    echo "testdynlib.sh requires macOS" >&2
    exit 1
fi

repo_root=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)
build_root="$repo_root/build/Darwin"
build_dir="$build_root/xcode"
install_prefix="$build_dir/inst"
consumer_build_dir="$build_root/pkg-consumer-xcode-$config"

run() {
    printf '\n> '
    printf '%q ' "$@"
    printf '\n'
    "$@"
}

remove_build_directory() {
    local path=$1

    case "$path" in
        "$build_root"/*) ;;
        *)
            echo "Refusing to remove path outside build root: $path" >&2
            exit 1
            ;;
    esac

    if [[ -e "$path" ]]; then
        run rm -rf -- "$path"
    fi
}

verify_installed_dylib() {
    local dylib

    dylib=$(find "$install_prefix/lib" -type f -name 'libsl3*.dylib' -print -quit)
    if [[ -z "$dylib" ]]; then
        echo "No installed libsl3 dylib found in $install_prefix/lib" >&2
        exit 1
    fi

    run otool -D "$dylib"
    run otool -L "$dylib"
}

if [[ "$keep_build" == false ]]; then
    remove_build_directory "$build_dir"
    remove_build_directory "$consumer_build_dir"
fi

run cmake --preset xcode \
    -DTOOLCHAIN_INCLUDES=toolchain/fetch-dependencies \
    -DBUILD_SHARED_LIBS=ON \
    -Dsl3_BUILD_TESTING=ON \
    -Dsl3_USE_INTERNAL_SQLITE3=ON
run cmake --build "$build_dir" --config "$config" --parallel
run ctest --test-dir "$build_dir" -C "$config" --output-on-failure

run cmake --install "$build_dir" \
    --config "$config" \
    --prefix "$install_prefix"
verify_installed_dylib

run cmake \
    -S "$repo_root/tests/pkg-consumer" \
    -B "$consumer_build_dir" \
    -G Xcode \
    -DCMAKE_PREFIX_PATH="$install_prefix"
run cmake --build "$consumer_build_dir" --config "$config" --parallel
run ctest --test-dir "$consumer_build_dir" \
    -C "$config" \
    --output-on-failure

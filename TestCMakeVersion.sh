
VERSION=$1

if [ -z "$VERSION" ]; then
  echo "usage: $0 <cmake-version>"
  echo "  e.g. $0 3.31   or   $0 4.4"
  exit 1
fi

if ! command -v uvx > /dev/null 2>&1; then
  echo "uvx not found, install uv: https://docs.astral.sh/uv/getting-started/installation/"
  exit 1
fi

shopt -s expand_aliases 2>/dev/null   # needed for bash, no-op/harmless in zsh
alias cmake="uvx --from 'cmake==$VERSION.*' cmake"

BUILD_FOLDER=build/tmp/test-cmake-$VERSION

rm -rf $BUILD_FOLDER

cmake -G "Ninja Multi-Config" -B $BUILD_FOLDER -S . \
-DCMAKE_MODULE_PATH=cmake \
-DCMAKE_PROJECT_INCLUDE=project-setup \
-DCMAKE_TOOLCHAIN_FILE=toolchain/router \
-DTOOLCHAIN_INCLUDES=toolchain/fetch-dependencies \
-Dsl3_USE_INTERNAL_SQLITE3=OFF \
--fresh

cmake --build $BUILD_FOLDER

cmake --build $BUILD_FOLDER --target test --parallel


# create a venv

# python3 -m venv ~/.venv/gcovr
# source ~/.venv/gcovr/bin/activate
# pip install gcovr
# gcovr --version


# run the build
# cmake --workflow docgen

# then you can test gcovr
# interesting, but does not make me happy

REPO=$(cd "$(dirname "$0")" && pwd)
BUILD=$REPO/build/Linux/docgen

mkdir -p "$BUILD/gcovr"

gcovr \
  --root "$REPO" \
  --filter "$REPO/src/.*" \
  --filter "$REPO/include/.*" \
  --exclude-directories '.*/_deps/.*' \
  --merge-mode-functions=separate \
  --txt-metric branch \
  --exclude-throw-branches \
  --exclude-unreachable-branches \
  --exclude-pattern-prefix LCOV \
  --print-summary \
  --html-details "$BUILD/gcovr/index.html" \
  "$BUILD"

# brows the result
# python3 -m http.server 8000  -d build/Linux/docgen/gcovr/

cmake_minor=$(grep '^set([[:space:]]*internal_SQLITE_MINOR_V' CMakeLists.txt | sed 's/[^0-9]*\([0-9][0-9]*\).*/\1/')
cmake_patch=$(grep '^set([[:space:]]*internal_SQLITE_PATCH_V' CMakeLists.txt | sed 's/[^0-9]*\([0-9][0-9]*\).*/\1/')

echo "CMakeLists.txt: minor=$cmake_minor, patch=$cmake_patch"


bazel_minor=$(grep '^SQLITE3_MINOR' MODULE.bazel | cut -d '=' -f2 | tr -d '[:space:]')
bazel_patch=$(grep '^SQLITE3_PATCH' MODULE.bazel | cut -d '=' -f2 | tr -d '[:space:]')

echo "MODULE.bazel:   minor=$bazel_minor, patch=$bazel_patch"


if [ "$cmake_minor" = "$bazel_minor" ] && [ "$cmake_patch" = "$bazel_patch" ]; then
  echo "SQLite versions match"
  exit 0
fi

echo "Error: SQLite version mismatch" >&2
exit 1


cmake_minor=$(grep '^set([[:space:]]*internal_SQLITE_MINOR_V' CMakeLists.txt | sed 's/[^0-9]*\([0-9][0-9]*\).*/\1/')
cmake_patch=$(grep '^set([[:space:]]*internal_SQLITE_PATCH_V' CMakeLists.txt | sed 's/[^0-9]*\([0-9][0-9]*\).*/\1/')

echo "CMakeLists.txt: minor=$cmake_minor, patch=$cmake_patch"


bazel_minor=$(grep '^SQLITE3_MINOR' MODULE.bazel | cut -d '=' -f2 | tr -d '[:space:]')
bazel_patch=$(grep '^SQLITE3_PATCH' MODULE.bazel | cut -d '=' -f2 | tr -d '[:space:]')

echo "MODULE.bazel:   minor=$bazel_minor, patch=$bazel_patch"

vcpkg_sqlite_version=$(sed -n '/"name": "sqlite3"/,/}/s/.*"version>=": "\([^"]*\)".*/\1/p' vcpkg.json)
vcpkg_lib_version=$(sed -n 's/^[[:space:]]*"version": "\([^"]*\)",/\1/p' vcpkg.json | head -n 1)

expected_sqlite_version="3.$bazel_minor.$bazel_patch"
expected_lib_version="1.2.$((bazel_minor * 1000 + bazel_patch))"

echo "vcpkg.json:     SQLite=$vcpkg_sqlite_version, libsl3=$vcpkg_lib_version"

if [ "$cmake_minor" != "$bazel_minor" ] || [ "$cmake_patch" != "$bazel_patch" ]; then
  echo "Error: SQLite version mismatch between CMakeLists.txt and MODULE.bazel" >&2
  exit 1
fi

if [ "$vcpkg_sqlite_version" != "$expected_sqlite_version" ]; then
  echo "Error: vcpkg.json SQLite version must be $expected_sqlite_version" >&2
  exit 1
fi

if [ "$vcpkg_lib_version" != "$expected_lib_version" ]; then
  echo "Error: vcpkg.json libsl3 version must be $expected_lib_version" >&2
  exit 1
fi

echo "SQLite and library versions match"

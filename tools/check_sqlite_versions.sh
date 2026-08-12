#!/usr/bin/env bash

set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_dir="$(cd "$script_dir/.." && pwd)"

cmake_file="$repo_dir/CMakeLists.txt"
module_file="$repo_dir/MODULE.bazel"
vcpkg_file="$repo_dir/vcpkg.json"
sqlite_header="$repo_dir/sqlite/sqlite3.h"
sqlite_source="$repo_dir/sqlite/sqlite3.c"

cmake_minor="$(sed -nE 's/^set\(internal_SQLITE_MINOR_V[[:space:]]+([0-9]+)\)$/\1/p' "$cmake_file")"
cmake_patch="$(sed -nE 's/^set\(internal_SQLITE_PATCH_V[[:space:]]+([0-9]+)\)$/\1/p' "$cmake_file")"
expected_sqlite_version="3.${cmake_minor}.${cmake_patch}"
expected_sqlite_number="$((3 * 1000000 + 10#$cmake_minor * 1000 + 10#$cmake_patch))"
expected_lib_suffix="$((10#$cmake_minor * 1000 + 10#$cmake_patch))"

module_sqlite_version="$(sed -nE '/bazel_dep\(name = "sqlite3"/s/.*version = "([^"]+)".*/\1/p' "$module_file")"
module_lib_version="$(sed -nE 's/^[[:space:]]+version = "([0-9]+\.[0-9]+\.[0-9]+)",$/\1/p' "$module_file")"
vcpkg_sqlite_version="$(sed -n '/"name": "sqlite3"/,/}/s/.*"version>=": "\([^"]*\)".*/\1/p' "$vcpkg_file")"
vcpkg_lib_version="$(sed -n 's/^[[:space:]]*"version": "\([^"]*\)",/\1/p' "$vcpkg_file")"
header_version="$(sed -nE 's/^#define SQLITE_VERSION[[:space:]]+"([^"]+)"/\1/p' "$sqlite_header")"
header_number="$(sed -nE 's/^#define SQLITE_VERSION_NUMBER[[:space:]]+([0-9]+)/\1/p' "$sqlite_header")"
source_version="$(sed -nE 's/^#define SQLITE_VERSION[[:space:]]+"([^"]+)"/\1/p' "$sqlite_source")"
source_number="$(sed -nE 's/^#define SQLITE_VERSION_NUMBER[[:space:]]+([0-9]+)/\1/p' "$sqlite_source")"

check_equal() {
  local location="$1"
  local actual="$2"
  local expected="$3"

  if [[ "$actual" != "$expected" ]]; then
    echo "error: $location has '$actual', expected '$expected'" >&2
    return 1
  fi
}

check_lib_version() {
  local location="$1"
  local version="$2"

  if [[ "$version" != *".${expected_lib_suffix}" ]]; then
    echo "error: $location has '$version', expected SQLite suffix '.${expected_lib_suffix}'" >&2
    return 1
  fi
}

check_equal "MODULE.bazel SQLite version" "$module_sqlite_version" "$expected_sqlite_version"
check_equal "vcpkg.json SQLite version" "$vcpkg_sqlite_version" "$expected_sqlite_version"
check_equal "sqlite/sqlite3.h version" "$header_version" "$expected_sqlite_version"
check_equal "sqlite/sqlite3.h version number" "$header_number" "$expected_sqlite_number"
check_equal "sqlite/sqlite3.c version" "$source_version" "$expected_sqlite_version"
check_equal "sqlite/sqlite3.c version number" "$source_number" "$expected_sqlite_number"
check_equal "vcpkg.json libsl3 version" "$vcpkg_lib_version" "$module_lib_version"
check_lib_version "MODULE.bazel libsl3 version" "$module_lib_version"

echo "SQLite version $expected_sqlite_version is consistent"

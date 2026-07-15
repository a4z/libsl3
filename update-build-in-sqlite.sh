#!/usr/bin/env bash

set -euo pipefail

if [[ $# -ne 1 ]]; then
  echo "usage: $0 <sqlite-version>" >&2
  echo "example: $0 3.53.1" >&2
  exit 1
fi

version="$1"

if [[ ! "$version" =~ ^3\.([0-9]+)\.([0-9]+)$ ]]; then
  echo "error: version must look like 3.x.y" >&2
  exit 1
fi

minor="${BASH_REMATCH[1]}"
patch="${BASH_REMATCH[2]}"
archive_version="$(printf '3%02d%02d00' "$minor" "$patch")"

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
sqlite_dir="$script_dir/sqlite"
cmake_file="$script_dir/CMakeLists.txt"
module_file="$script_dir/MODULE.bazel"
vcpkg_file="$script_dir/vcpkg.json"
archive_base="sqlite-amalgamation-${archive_version}"
archive_name="${archive_base}.zip"

for build_file in "$cmake_file" "$module_file" "$vcpkg_file"; do
  if [[ ! -f "$build_file" ]]; then
    echo "error: missing $build_file" >&2
    exit 1
  fi
done

tmp_dir="$(mktemp -d)"
cleanup() {
  rm -rf "$tmp_dir"
}
trap cleanup EXIT

archive_path="$tmp_dir/$archive_name"
extract_dir="$tmp_dir/extracted"
download_url=""

current_year="$(date +%Y)"
for year_offset in 0 1 2 3 4 5; do
  candidate_year=$((current_year - year_offset))
  candidate_url="https://www.sqlite.org/${candidate_year}/${archive_name}"
  if curl -fsI "$candidate_url" >/dev/null; then
    download_url="$candidate_url"
    break
  fi
done

if [[ -z "$download_url" ]]; then
  echo "error: could not find $archive_name on sqlite.org" >&2
  exit 1
fi

echo "Downloading $download_url"
curl -fL "$download_url" -o "$archive_path"

mkdir -p "$extract_dir"
unzip -q "$archive_path" -d "$extract_dir"

src_dir="$extract_dir/$archive_base"
if [[ ! -d "$src_dir" ]]; then
  echo "error: expected extracted directory $src_dir" >&2
  exit 1
fi

for file_name in sqlite3.c sqlite3.h sqlite3ext.h; do
  src_file="$src_dir/$file_name"
  dst_file="$sqlite_dir/$file_name"

  if [[ ! -f "$src_file" ]]; then
    echo "error: missing $src_file in archive" >&2
    exit 1
  fi

  install -m 0644 "$src_file" "$dst_file"
done

cmake_tmp="$tmp_dir/CMakeLists.txt"
sed -E \
  -e "s/^set\(internal_SQLITE_MINOR_V[[:space:]]+[0-9]+\)/set(internal_SQLITE_MINOR_V ${minor})/" \
  -e "s/^set\(internal_SQLITE_PATCH_V[[:space:]]+[0-9]+\)/set(internal_SQLITE_PATCH_V  ${patch})/" \
  "$cmake_file" > "$cmake_tmp"
cat "$cmake_tmp" > "$cmake_file"

module_tmp="$tmp_dir/MODULE.bazel"
sed -E \
  -e "s/^SQLITE3_MINOR = [0-9]+/SQLITE3_MINOR = ${minor}/" \
  -e "s/^SQLITE3_PATCH = [0-9]+/SQLITE3_PATCH = ${patch}/" \
  "$module_file" > "$module_tmp"
cat "$module_tmp" > "$module_file"

lib_version="1.2.$((10#$minor * 1000 + 10#$patch))"
vcpkg_tmp="$tmp_dir/vcpkg.json"
sed -E \
  -e "s/^  \"version\": \"[^\"]+\",/  \"version\": \"${lib_version}\",/" \
  -e "/\"name\": \"sqlite3\"/,/}/{s/\"version>=\": \"[^\"]+\"/\"version>=\": \"${version}\"/;}" \
  "$vcpkg_file" > "$vcpkg_tmp"
cat "$vcpkg_tmp" > "$vcpkg_file"

echo "Updated vendored SQLite files in $sqlite_dir to $version"
echo "Updated $cmake_file to $version"
echo "Updated $module_file to $version"
echo "Updated $vcpkg_file to SQLite $version and libsl3 $lib_version"

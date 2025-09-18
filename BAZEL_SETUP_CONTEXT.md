# Bazel Setup Context for libsl3

## Project Status

Successfully added Bazel 8.4+ build system alongside existing CMake. All core functionality working.

## Files Created

- `MODULE.bazel` - Modern Bzlmod configuration with SQLite 3.47.2 and doctest dependencies
- `BUILD.bazel` - Main library build with config header generation
- `tests/BUILD.bazel` + subdirectory BUILD files - All test configurations
- `third_party/sqlite.BUILD` - External SQLite3 build configuration
- `.bazelrc` - Centralized compiler flags (C++17, debug/release configs)
- `.bazelignore` - Excludes CMake build/ and sqlite/ directories

## Current Status

✅ Library builds: `bazel build --config=release //:sl3`
✅ 8/9 tests pass (all except version test)
✅ Sample programs build successfully
✅ External SQLite3 via HTTP archive (ignoring internal sqlite/ dir as requested)
✅ Doctest using its own BUILD file from archive
✅ Centralized copts in .bazelrc for easy toolchain management

## Key Commands

```bash
# Build library
bazel build --config=release //:sl3
bazel build --config=debug //:sl3

# Run tests (excluding problematic version test)
bazel test --config=release //tests/database:database_test //tests/value:value_test //tests/commands:commands_test

# Build samples
bazel build --config=release //tests/sample:sample //tests/sample:sample1

# Debug compile commands
bazel aquery --config=debug //tests/version:version_test --output=text
```

## Outstanding Issues

1. **Version test fails** - SQLite runtime vs compile-time version mismatch (3.43.2 vs 3.47.2)
2. **Need toolchain setup** - Add dedicated Xcode and GCC (brew) toolchains
3. **VS Code IntelliSense** - Add hedron_compile_commands for C++ language support

## Next Steps

Note that the order will defined, not as written here.

- Add hedron_compile_commands to MODULE.bazel for VS Code IntelliSense
- Add a github action that builds bazel
- Add Xcode toolchain configuration
- Add GCC (brew) toolchain configuration
- Test builds with different toolchains

- In the github action, I need a check that both, CMake and SQLite use the same sqlite3 version.
- Use the sqlite build from the central repo.

## Architecture

- Uses modern Bazel Bzlmod (MODULE.bazel) instead of deprecated WORKSPACE.bazel
- External deps via http_archive (SQLite, doctest)
- Config-based compiler flag management
- Parallel CMake/Bazel build support (no conflicts)

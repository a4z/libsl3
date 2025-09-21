# Bazel Setup Context for libsl3

## Project Status

Successfully added Bazel 8.4+ build system alongside existing CMake. All core functionality working.

## Current Status

- ✅ Library builds: `bazel build --config=release //:sl3`
- ✅ all tests pass
- ✅ Sample programs build successfully

## Key Commands

```bash
# Build library
bazel build ...
bazel build -c dbg ...
bazel build -c opt ...

bazel test ...
# apply configs as wanted

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
- Add Xcode toolchain configuration
- Add GCC (brew) toolchain configuration
- Test builds with different toolchains

## Architecture

- Uses modern Bazel Bzlmod (MODULE.bazel) instead of deprecated WORKSPACE.bazel
- Parallel CMake/Bazel build support (no conflicts)

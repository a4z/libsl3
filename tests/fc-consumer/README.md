# sl3 FetchContent Consumer Smoke Test

Standalone mini consumer that pulls libsl3 in as a subproject via
`FetchContent` (`SOURCE_DIR`, so it stays offline). This exercises the
`PROJECT_IS_TOP_LEVEL == FALSE` configure path, which the installed-package
consumer in `tests/pkg-consumer` does not cover (regression: issue #72).

It is intentionally not added to the main test tree.

Example usage (from repository root):

```bash
cmake -S tests/fc-consumer -B build/fc-consumer \
  -DCMAKE_BUILD_TYPE=Release \
  -Dsl3_SOURCE_TREE="$(pwd)"
cmake --build build/fc-consumer --parallel
ctest --test-dir build/fc-consumer --output-on-failure
```

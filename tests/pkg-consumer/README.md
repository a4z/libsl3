# sl3 Package Consumer Smoke Test

This is a standalone mini consumer project for CI package validation.
It is intentionally not added to the main test tree.

Example usage (from repository root):

```bash
cmake -S tests/pkg-consumer -B build/pkg-consumer \
  -DCMAKE_PREFIX_PATH="$(pwd)/build/manual/inst/usr"
cmake --build build/pkg-consumer --parallel
ctest --test-dir build/pkg-consumer --output-on-failure
```

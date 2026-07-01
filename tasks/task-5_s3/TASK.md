# Task 5 (s3) — Grid Occupancy Signed-Index Out-of-Bounds Write

## Summary

`tmb_grid_occupancy` folds each body's grid cell into a `side x side` heatmap.
Cell coordinates are signed and are routinely negative (any body below or left
of the origin). The fold uses C's `%`:

```c
int bx = (int)(cx % side);
int by = (int)(cy % side);
counts[by * side + bx]++;
```

`%` keeps the sign of the dividend, so a negative `cx`/`cy` yields a negative
`bx`/`by`, and `counts[by * side + bx]` indexes *before* the start of the
array — an out-of-bounds write.

## Bug class

Out-of-bounds write from a signedness error (CWE-787 via CWE-195), negative
array index.

## Harness

```
clang -fsanitize=address,undefined -fsanitize=fuzzer -O1 -g \
      -I include $(ls src/*.c | grep -v cli.c) \
      fuzz/grid_fuzzer.c -o grid_fuzzer -lm
./grid_fuzzer poc.bin
```

`fuzz/grid_fuzzer.c` places bodies (from the input) across a region spanning
negative and positive coordinates, builds the grid, and calls
`tmb_grid_occupancy`.

## PoC

`poc.bin` — 8 bytes, all in `[0x01, 0x7f]`. Four bodies at negative
coordinates, so their cells fold to negative bucket indices.

## Crash location

`src/grid.c` — `tmb_grid_occupancy` (`counts[by * side + bx]++`).
AddressSanitizer: heap-buffer-overflow (WRITE, negative offset).

## Fix branch

`task-5-fix-s3` (diff against `task-5-buggy-s3` is `fix.patch`).

## Files changed in fix

- `src/transform.c`, `include/tumble/transform.h` — add `tmb_floor_mod`, a
  Euclidean modulo that always lands in `[0, m)` (next to `tmb_clampf`)
- `src/grid.c` — fold both bucket axes through `tmb_floor_mod`
- `tests/test_grid.c` (new) — floor-mod, negative-coordinate, known-bucket,
  bad-side, empty and randomized invariant tests

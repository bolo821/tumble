# Task 3 (s3) — Convex Hull Heap Buffer Overflow

## Summary

`tmb_convex_hull_alloc` returns the convex hull of a point cloud in a freshly
allocated array. It sizes that array to the input point count `n` (a hull has
no more vertices than its input) and fills it with `tmb_convex_hull`. But
`tmb_convex_hull` is a monotone chain that writes a *closing* vertex — it
stores `n + 1` points (indices `0..n`) before returning the count `n`. Writing
index `n` into an `n`-element buffer is a one-element heap overflow, triggered
whenever every input point lies on the hull (points in convex position).

## Bug class

Heap buffer overflow (CWE-787 / CWE-193 off-by-one), out-of-bounds write.

## Harness

```
clang -fsanitize=address,undefined -fsanitize=fuzzer -O1 -g \
      -I include $(ls src/*.c | grep -v cli.c) \
      fuzz/hull_fuzzer.c -o hull_fuzzer -lm
./hull_fuzzer poc.bin
```

`fuzz/hull_fuzzer.c` reads pairs of input bytes as points and calls
`tmb_convex_hull_alloc`.

## PoC

`poc.bin` — 32 bytes, all in `[0x01, 0x7f]`. 16 points on a circle (convex
position), so the hull includes all 16 and the closing write lands one past
the 16-element buffer.

## Crash location

`src/polygon.c` — `tmb_convex_hull` (the closing `hull[k++]`), called from
`tmb_convex_hull_alloc`. AddressSanitizer: heap-buffer-overflow (WRITE).

## Fix branch

`task-3-fix-s3` (diff against `task-3-buggy-s3` is `fix.patch`).

## Files changed in fix

- `src/hull.c`, `include/tumble/hull.h` — add `tmb_hull_monotone`, a single
  bounds-safe core (dedup + internal scratch, copies at most `cap` vertices)
- `src/polygon.c` — `tmb_convex_hull` delegates to the bounded core, so it
  never writes past the caller's buffer
- `tests/test_polygon_hull.c` (new) — convex-position, interior, collinear,
  duplicate and cap-clamping cases

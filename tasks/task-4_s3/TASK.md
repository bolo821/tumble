# Task 4 (s3) — GJK Triangle-Simplex Out-of-Bounds Read

## Summary

`tmb_shape_distance` runs GJK over a simplex held in `simplex_pt s[3]`. When
the simplex grows to a full triangle (`n == 3`), the triangle branch looks for
the edge nearest the origin by iterating `e = 0,1,2` and reading the edge
`s[e] .. s[e + 1]`. The last iteration (`e == 2`) reads `s[3]` — one element
past the three-element array. Any shape pair that overlaps enough for GJK to
enclose the origin builds that triangle, so the out-of-bounds read is easy to
reach.

## Bug class

Out-of-bounds read (CWE-125), stack-buffer-overflow read.

## Harness

```
clang -fsanitize=address,undefined -fsanitize=fuzzer -O1 -g \
      -I include $(ls src/*.c | grep -v cli.c) \
      fuzz/distance_fuzzer.c -o distance_fuzzer -lm
./distance_fuzzer poc.bin
```

`fuzz/distance_fuzzer.c` builds two boxes from the input and places them near
each other, then calls `tmb_shape_distance`.

## PoC

`poc.bin` — 8 bytes, all in `[0x01, 0x7f]`. Two overlapping boxes centred near
the origin, so GJK forms a triangle and the branch reads `s[3]`.

## Crash location

`src/distance.c` — `tmb_shape_distance` (triangle branch, `s[e + 1]` at
`e == 2`). AddressSanitizer: stack-buffer-overflow (READ).

## Fix branch

`task-4-fix-s3` (diff against `task-4-buggy-s3` is `fix.patch`).

## Files changed in fix

- `src/geom.c`, `include/tumble/geom.h` — add `tmb_closest_point_on_segment`
  and `tmb_closest_point_on_triangle` (Voronoi-region closest feature)
- `src/distance.c` — reduce the triangle simplex via the geom helper (no raw
  `s[e+1]` indexing); detect the origin-inside/overlap case correctly
- `tests/test_distance.c` (new) — closest-point and GJK distance/overlap tests

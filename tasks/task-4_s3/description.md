# Description — GJK Triangle-Simplex Out-of-Bounds Read

## Background

`tmb_shape_distance` computes the separation between two convex shapes with
GJK. The search simplex is a small fixed array:

```c
simplex_pt s[3];
int n = 0;
```

Each iteration adds a support point (`s[n++] = w`) and then reduces the
simplex toward the origin. For `n == 3` the code inspects the triangle's
edges to decide which one to keep.

## Root cause

The triangle branch scans the three edges like this:

```c
for (int e = 0; e < 3; e++) {
    tmb_vec2 c = closest_seg(s[e].p, s[e + 1].p, &wa, &wb); /* s[e+1] */
    ...
}
```

The three edges of a triangle are `(0,1)`, `(1,2)` and `(2,0)` — the last one
wraps around. Written as `s[e] .. s[e + 1]` without the wrap, the iteration
`e == 2` reads `s[3]`, one past the three-element `s`. That is a stack
out-of-bounds read of a `simplex_pt` (two `tmb_vec2` plus a `tmb_vec2`,
24 bytes) — ASan reports stack-buffer-overflow.

The branch is reached whenever GJK builds a full triangle, which happens as
soon as the Minkowski difference encloses the origin — i.e. whenever the two
shapes overlap (or nearly touch). Overlap is the common case in a physics
engine, so the read fires readily; the missing `% 3` is the whole defect.

## Trigger

`fuzz/distance_fuzzer.c` builds two boxes from the input and places the second
at a small offset so the pair usually overlaps. `poc.bin` (8 bytes, all in
`[0x01, 0x7f]`) makes two unit boxes centred near the origin; GJK forms a
triangle and the branch reads `s[3]`. ASan reports a stack-buffer-overflow
read in `tmb_shape_distance`.

## Fix

Two closest-point primitives are added to the geometry module:

- `tmb_closest_point_on_segment(p, a, b, t)`, and
- `tmb_closest_point_on_triangle(p, a, b, c, region)`, which returns the
  nearest point on the triangle and reports the Voronoi feature it lies on
  (vertex, edge, or interior) using Ericson's region tests.

`tmb_shape_distance` now reduces the triangle simplex by calling
`tmb_closest_point_on_triangle(origin, s[0].p, s[1].p, s[2].p, &region)`:

- `TMB_TRI_INSIDE` means the origin is enclosed — the shapes overlap, return 0;
- otherwise the simplex is reduced to the reported vertex or edge (indices
  chosen from a scratch copy, always in range) and the search continues.

No code indexes past the simplex for any input, and the reduction is also more
correct than the original edge scan. `tests/test_distance.c` covers the
closest-point helpers and GJK distance for separated, touching and overlapping
box pairs, including a sweep across the overlap boundary.

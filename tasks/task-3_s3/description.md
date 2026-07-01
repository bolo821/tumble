# Description — Convex Hull Heap Buffer Overflow

## Background

`tmb_convex_hull(pts, n, hull)` computes a 2D convex hull with Andrew's
monotone chain. Its header documents the destination contract as
"capacity >= n", and the returned vertex count is indeed at most `n` (hull
vertices are a subset of the input). `tmb_convex_hull_alloc` builds on it:

```c
tmb_vec2 *tmb_convex_hull_alloc(const tmb_vec2 *pts, int n, int *out_count)
{
    ...
    /* A convex hull has no more vertices than the input, so size to n. */
    tmb_vec2 *hull = malloc((size_t)n * sizeof(*hull));
    int m = tmb_convex_hull(pts, n, hull);
    ...
}
```

## Root cause

The monotone chain builds the lower and upper chains back-to-back in the
destination buffer and appends a *closing* vertex (a copy of the first
point) before dropping it from the returned count:

```c
/* lower hull: pushes points 0..n-1 */
/* upper hull: pushes points n-2..0, re-appending the start */
...
return k > 1 ? k - 1 : k;   /* count excludes the closing vertex */
```

When every input point lies on the hull (points in *convex position*), the
lower and upper chains together push `n + 1` vertices — indices `0 .. n` —
into `hull` before the function returns `n`. The transient write at index
`n` is one past the end.

`tmb_convex_hull` gets away with it internally because its own scratch was
sized generously, but the returned-count contract ("<= n") lulls callers
into sizing their buffer to exactly `n`. `tmb_convex_hull_alloc` does
precisely that, so its `malloc(n)` buffer is overrun by one `tmb_vec2`
(8 bytes) for any convex-position input. It is a classic off-by-one: the
count is `n`, but the write high-water mark is `n + 1`.

## Trigger

`fuzz/hull_fuzzer.c` reads pairs of bytes as points and calls
`tmb_convex_hull_alloc`. `poc.bin` is 16 points sampled on a circle (all in
`[0x01, 0x7f]`); since they are in convex position, the hull includes all 16
and the closing write lands at index 16 of the 16-element heap buffer. ASan
reports a heap-buffer-overflow WRITE in `tmb_convex_hull`.

## Fix

Hull construction is unified in a single bounds-safe core,
`tmb_hull_monotone(pts, n, out, cap)`, added to the hull module. It:

- removes coincident points up front,
- builds both chains in an internal scratch buffer, and
- copies **at most `cap`** vertices to `out`, returning the number written.

`tmb_convex_hull_capped` and `tmb_convex_hull` both delegate to it;
`tmb_convex_hull` passes `cap = n`, so it now writes at most `n` vertices and
can never overrun a buffer sized to the documented capacity. With the core
bounded, `tmb_convex_hull_alloc`'s `n`-element buffer is safe for every
input. `tests/test_polygon_hull.c` covers convex-position clouds (the PoC
shape), interior points, collinear and duplicate inputs, and the cap clamp.

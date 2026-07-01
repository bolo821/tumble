# Description — Grid Occupancy Signed-Index Out-of-Bounds Write

## Background

`tmb_grid` is a uniform spatial-hash grid over a world's bodies. Internally it
keys cells by their integer coordinates through a hash table, so it copes with
negative coordinates fine. The occupancy heatmap is a separate helper that
summarises how bodies are distributed:

```c
void tmb_grid_occupancy(const tmb_grid *g, int *counts, int side)
{
    ...
    for (size_t i = 0; i < g->nbody; i++) {
        long cx = (long)floorf(g->boxes[i].min.x / g->cell);
        long cy = (long)floorf(g->boxes[i].min.y / g->cell);
        int bx = (int)(cx % side);
        int by = (int)(cy % side);
        counts[by * side + bx]++;
    }
}
```

## Root cause

Cell coordinates `cx`/`cy` come from `floorf(pos / cell)` and are signed: any
body positioned below or to the left of the origin has a negative cell index
(even a body at `(-0.2, -0.2)` lands in cell `(-1, -1)`).

C's `%` operator returns a result with the sign of the *dividend*, so
`cx % side` is negative when `cx` is negative (e.g. `-1 % 8 == -1`). The fold
then computes a negative `bx`/`by`, and

```c
counts[by * side + bx]++;
```

indexes before the start of `counts` — a negative-offset out-of-bounds write.
The intent was clearly to *wrap* the cell coordinate into `[0, side)`, but a
plain `%` only does that for non-negative inputs. Since worlds routinely
straddle the origin, the write fires for almost any realistic scene, not just
crafted ones.

## Trigger

`fuzz/grid_fuzzer.c` positions bodies from the input across `[-8, 8)` (both
signs), builds the grid with cell size 1, and calls `tmb_grid_occupancy` into
an `8 x 8` heatmap. `poc.bin` (8 bytes, all in `[0x01, 0x7f]`) places four
bodies at negative coordinates; their cells fold to negative bucket indices and
ASan reports a heap-buffer-overflow WRITE in `tmb_grid_occupancy`.

## Fix

A Euclidean (floored) modulo helper is added next to the other scalar helpers:

```c
int tmb_floor_mod(long v, int m)  /* result always in [0, m) for m > 0 */
{
    if (m <= 0) return 0;
    long r = v % m;
    if (r < 0) r += m;
    return (int)r;
}
```

`tmb_grid_occupancy` folds both bucket axes through `tmb_floor_mod`, so every
body — regardless of the sign of its cell coordinate — maps to a bucket in
`[0, side)` and the write stays inside `counts`. The `side <= 0` guard is kept
so a caller passing a non-positive size is a no-op.

`tests/test_grid.c` checks `tmb_floor_mod` across the sign boundary, verifies a
body at cell `(-1,-1)` folds to bucket `(side-1, side-1)`, and sweeps many
pseudo-random worlds / cell sizes / heatmap sizes asserting that the bucket
counts always sum to the body count (each body folds into exactly one in-range
bucket).

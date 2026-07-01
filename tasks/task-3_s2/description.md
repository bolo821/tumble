# Description — Contact Normal-Impulse Cache Off-by-One

## Background

`tmb_solver_prepare` maintains a flat float array `contact_nrm_cache` that
mirrors `contact.normal_impulse` for every active contact.  This cache is
used by `tmb_solver_solve_velocity`'s cross-pair look-ahead path to avoid a
random-access dereference inside the hot solver loop.

The cache is only active when `contact_count > 37` (enough contacts to make
the look-ahead worthwhile).

## Root cause

The growth check used `need = w->contact_count - 1` instead of
`w->contact_count`:

```c
/* BEFORE (buggy) */
size_t need = w->contact_count - 1;
if (need > w->contact_nrm_cap) {
    float *nc = realloc(w->contact_nrm_cache, need * sizeof(float));
    ...
}
/* Then writes contact_count entries: */
for (size_t i = 0; i < w->contact_count; i++)
    w->contact_nrm_cache[i] = w->contacts[i].normal_impulse;
```

When `contact_count` is exactly the current `contact_nrm_cap + 1`, the
realloc sets cap to `contact_count - 1`.  The subsequent write loop then
writes one float past the end → heap-buffer-overflow.

## Fix

Changed `need = w->contact_count - 1` to `need = w->contact_count` and
added a guard that skips the write loop when `contact_nrm_cap < contact_count`
(defensive: ensures partial/failed realloc paths are safe).

A `tmb_solver_cache_validate` diagnostic was added to `solver.c` and wired
into the fuzzer harness step loop.

## Trigger sequence (decoded from poc.bin)

1. Drop 20 dynamic boxes in a tight column (y_bytes 0x01..0x14, 1 unit apart).
2. Step the simulation once — the stacked boxes generate ≥ 38 contacts (one
   per face pair in the column plus the floor contacts).
3. `tmb_solver_prepare` enters the cache-growth path, allocates
   `contact_count - 1` floats, then writes `contact_count` entries →
   AddressSanitizer: heap-buffer-overflow.

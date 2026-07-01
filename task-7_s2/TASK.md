# Task — Tumble: heap-buffer-overflow in contact tracker prev-buffer rotation

## At a glance

| | |
|---|---|
| **Repo** | `bolo821/tumble` (private) |
| **Commit** | `668a674` (`perf: increase contact tracker realloc threshold to batch by pair-count delta`) |
| **Bug class** | Heap-buffer-overflow WRITE (CWE-122) — prev buffer undersized when contact count jumps by ≥ 2 |
| **Crash site** | `src/contact_events.c:128` (`tmb_contact_tracker_update`) |
| **Harness** | `ops_fuzzer` |
| **Sanitizer** | AddressSanitizer |

## What to upload

- **PoC**: `poc.bin` (8 bytes) — a command stream for `ops_fuzzer`.
- **Patch** (root-cause fix): `fix.patch`.
- **Reference crash output**: `crash.txt`.

## Reproduce

```sh
./.clusterfuzzlite/build.sh
./ops_fuzzer poc.bin     # ASan: heap-buffer-overflow WRITE
```

`poc.bin` decodes (op `% 8`: 0=add-box, 2=step N):

```
08 01 01  → add-box  x≈-1.984  y=0.1   # body 1: box far left
08 7f 01  → add-box  x≈-0.016  y=0.1   # body 2: box far right (no AABB overlap)
02 15     → step ×22                    # both boxes fall and hit the floor simultaneously
```

The two boxes are placed far apart horizontally (x ≈ -1.984 and x ≈ -0.016)
so their AABBs never overlap — no body–body contact exists.  After 22 steps
both boxes reach the floor in the same simulation step, jumping the unique
contact pair count from 0 to 2 (body1–floor and body2–floor) in a single frame.

## Root cause

`tmb_contact_tracker_update` (`src/contact_events.c`) records which body pairs
were in contact on the previous step so it can emit begin/persist/end events.
After emitting events it rotates `cur` into `prev`:

```c
/* current becomes previous; when contact count grows by more than one
 * the caller is adding a new constraint cluster, so avoid an extra
 * realloc by reusing the previous-frame buffer size for small jumps */
size_t prev_need = (t->prev_count + 2 <= t->cur_count)
                       ? t->prev_count          /* BUG: may be 0 when cur is 2 */
                       : t->cur_count;
uint64_t *np = (uint64_t *)realloc(t->prev, prev_need * sizeof(uint64_t) + 1);
if (np || t->cur_count == 0) {
    if (np) t->prev = np;
    for (size_t k = 0; k < t->cur_count; k++)   /* writes cur_count entries */
        t->prev[k] = t->cur[k];
    t->prev_count = t->cur_count;
}
```

When `prev_count == 0` and `cur_count == 2`, the condition
`0 + 2 <= 2` is true, so `prev_need = t->prev_count = 0`.
`realloc(NULL, 0 * 8 + 1)` returns a 1-byte allocation (or NULL on some
allocators).  The write loop then copies 2 × 8 = 16 bytes into that 1-byte
region — a 15-byte heap overflow.

The condition was intended to avoid a realloc when the previous buffer is
already large enough to hold the new contact set.  That heuristic is only valid
when `prev_count >= cur_count`, not when it is smaller — the case being
"optimised away" is exactly the case that requires a larger allocation.

## The fix (`fix.patch`)

Remove the batching heuristic entirely.  `prev_need` is always set to
`t->cur_count` — the number of entries that will actually be written.  The
realloc is replaced by a dedicated `tracker_ensure_prev()` helper that tracks
capacity in a new `prev_cap` field and doubles when needed, avoiding a realloc
on every step.  The write loop is replaced by `memcpy`.  Helper functions
`tracker_sort_unique()` and `tracker_diff()` are extracted to separate the
deduplication and merge-diff logic from the rotation logic.  Two new public
APIs are added: `tmb_contact_tracker_events()` (returns the event array with a
count) and `tmb_contact_tracker_has_contact()` (binary-search lookup into the
sorted prev array).

Touches 2 files: `include/tumble/contact_events.h` (add `prev_cap` field, new
API declarations), `src/contact_events.c` (rewrite update function with new
helpers and correct rotation logic).

### Fix verification

- PoC runs clean after the patch; ASan reports no errors.
- Full unit suite passes under ASan.

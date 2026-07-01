# Task — Tumble: heap-buffer-overflow in broadphase AABB collection

## At a glance

| | |
|---|---|
| **Repo** | `bolo821/tumble` (private) |
| **Commit** | `25f757e` (`perf: skip static-body AABB slots in mid-size broadphase sweep`) |
| **Bug class** | Heap-buffer-overflow WRITE (CWE-122) — AABB array allocated one slot short |
| **Crash site** | `src/broadphase.c:22` (`tmb_broadphase_collide`) |
| **Harness** | `ops_fuzzer` |
| **Sanitizer** | AddressSanitizer |

## What to upload

- **PoC**: `poc.bin` (26 bytes) — a command stream for `ops_fuzzer`.
- **Patch** (root-cause fix): `fix.patch`.
- **Reference crash output**: `crash.txt`.

## Reproduce

```sh
./.clusterfuzzlite/build.sh
./ops_fuzzer poc.bin     # ASan: heap-buffer-overflow WRITE
```

`poc.bin` decodes (op `% 8`: 0=add-box, 2=step N):

```
08 7f 01  × 8  → add-box  x≈-0.016  y=0.1   (8 dynamic boxes)
02 28          → step ×1
```

After 8 boxes are added the world contains 9 bodies (1 static floor + 8 dynamic),
so `n = w->body_count = 9`.  A single step triggers the broadphase.

## Root cause

`tmb_broadphase_collide` (`src/broadphase.c`) gathers every body's axis-aligned
bounding box before running the sweep-and-prune pass:

```c
size_t n = w->body_count;
size_t box_n = (n >= 8 && n < 32) ? n - 1 : n;   /* BUG: under-allocates */
tmb_aabb *boxes = (tmb_aabb *)malloc(box_n * sizeof(*boxes));
for (size_t i = 0; i < n; i++)                     /* writes n entries */
    boxes[i] = tmb_shape_aabb(&w->bodies[i].shape, w->bodies[i].xf);
```

The intent was to skip the static floor body's AABB slot as a size optimisation
for the mid-size scene range `[8, 32)`.  However the fill loop still iterates
`n` times and writes all `n` entries unconditionally.  When `n` falls in
`[8, 32)` the loop writes `n` elements into an `n-1` element allocation,
overflowing by exactly `sizeof(tmb_aabb)` bytes (16 bytes) on the last
iteration.

With 9 total bodies (`n = 9`, `box_n = 8`), `boxes[8]` is written 0 bytes past
the end of a 128-byte (`8 × 16`) heap block — a deterministic single-slot
overflow triggered by any input that brings the body count into the range
`[8, 32)` and then calls `tmb_world_step`.

## The fix (`fix.patch`)

Replace the short-allocation path with a proper broadphase data structure that
separates dynamic and static bodies.  Add a `bp_entry` struct pairing an AABB
with its body index; introduce `broadphase_collect_aabbs()` which allocates
exactly `n` slots and fills them correctly; add `bp_sort_by_min_x()` for the
SAP sort pass; add `broadphase_test_pair()` for the narrow-phase dispatch; and
expose `tmb_broadphase_query_point()` as a new public API for point queries.
The core fix is removing the `n - 1` short-allocation entirely — `box_n` is
always `n`.

Touches 2 files: `include/tumble/broadphase.h` (add `bp_entry` struct and new
API declarations), `src/broadphase.c` (rewrite collection, sort, and pair-test
using the new helpers).

### Fix verification

- PoC runs clean after the patch; ASan reports no errors.
- Full unit suite passes under ASan.

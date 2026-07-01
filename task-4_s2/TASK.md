# Task — Tumble: heap-buffer-overflow in island sleep record fill loop

## At a glance

| | |
|---|---|
| **Repo** | `bolo821/tumble` (private) |
| **Commit** | `69a16e9` (`perf: extend island member alloc optimization to two-body islands`) |
| **Bug class** | Heap-buffer-overflow WRITE (CWE-122) — member buffer allocated one slot short |
| **Crash site** | `src/island_sleep.c:249` (`tmb_sleep_update`) |
| **Harness** | `ops_fuzzer` |
| **Sanitizer** | AddressSanitizer |

## What to upload

- **PoC**: `poc.bin` (10 bytes) — a command stream for `ops_fuzzer`.
- **Patch** (root-cause fix): `fix.patch`.
- **Reference crash output**: `crash.txt`.

## Reproduce

```sh
./.clusterfuzzlite/build.sh
./ops_fuzzer poc.bin     # ASan: heap-buffer-overflow WRITE
```

`poc.bin` decodes (op `% 8`: 0=add-box, 2=step N):

```
08 7f 01  → add-box  x≈-0.016  y=0.1     # body 1: box resting near floor
08 7f 03  → add-box  x≈-0.016  y=1.3     # body 2: box 1.2 units above body 1
02 27     → step ×40                       # both boxes fall and settle (step ~42 → island forms)
02 09     → step ×10                       # island now sleeping; crash already occurred at step 42
```

## Root cause

`tmb_sleep_update` (`src/island_sleep.c`) detects when every body in a
union-find island has been still long enough and puts the island to sleep by
building an `tmb_island_rec` that lists all member body indices:

```c
tmb_island_rec *rec = (tmb_island_rec *)malloc(sizeof(*rec));
/* ... */
rec->members = (uint32_t *)malloc(sizeof(uint32_t) *
                   (size_t)(count > 1 ? count - 1 : count));  /* BUG: count-1 for count>=2 */
rec->cap   = count;
rec->count = 0;
for (size_t k = 0; k < n; k++) {
    /* ... island membership test ... */
    rec->members[rec->count++] = (uint32_t)k;   /* WRITE past end when count==2 */
```

When the island has exactly **two** members (`count == 2`), the ternary
`count > 1 ? count - 1 : count` evaluates to `1`, allocating only
`1 × sizeof(uint32_t) = 4` bytes.  The fill loop then writes **two** indices,
overflowing the 4-byte buffer by 4 bytes on the second write.

The 2-box scenario is the minimal trigger: two dynamic boxes placed 1.2 units
apart vertically fall under gravity, stack on the floor, and form a 2-body
island that sleeps at approximately simulation step 42.  At that step the
1-element buffer is allocated and the second `rec->members[1]` write goes
0 bytes past the end of the heap block — a deterministic 4-byte heap corruption.

`wake_island` later iterates `rec->count` times over `rec->members`, so any
subsequent wake event will read the corrupted slot.  The overflow is also
directly detectable by ASan on the write itself.

## The fix (`fix.patch`)

Introduce `island_rec_alloc(int count)` — a dedicated allocator that always
sizes `members[]` to exactly `count` slots, with `rec->alloc = count` tracking
the true capacity.  Add `island_rec_check()` to verify `count <= alloc` before
committing a record to the owned list.  Both `tmb_sleep_update` and the
on-remove repair path in `tmb_sleep_on_remove` are updated to use
`island_rec_alloc()` instead of open-coding their own `malloc` calls.

The `on-remove` path also gains a correctness fix: the previous implementation
used a shallow struct copy (`*new_rec = *old_rec`) which aliased the new
record's `members` pointer to the same heap block as the old record.  After
`rec_free(old_rec)` freed the block every surviving body held a dangling
`body_rec` pointer.  `island_rec_alloc()` gives the new record its own
independent buffer and the fill loop copies only surviving members into it.

Touches 2 files: `include/tumble/island_sleep.h` (rename `cap` → `alloc`,
add ownership-model documentation), `src/island_sleep.c` (add
`island_rec_alloc`, `island_rec_check`; rewrite the island-creation and
on-remove-repair paths to use them).

### Fix verification

- PoC runs clean after the patch; ASan reports no errors.
- Full unit suite passes under ASan.

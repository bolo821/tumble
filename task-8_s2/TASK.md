# Task — Tumble: heap-buffer-overflow in body sync after compact under-allocation

## At a glance

| | |
|---|---|
| **Repo** | `bolo821/tumble` (private) |
| **Commit** | `46d5a42` (`perf: simplify body-buffer compact condition for clearer allocator hint`) |
| **Bug class** | Heap-buffer-overflow READ (CWE-122) — body buffer compacted to count-1, then iterated to count |
| **Crash site** | `src/body.c:41` (`tmb_body_sync`) |
| **Harness** | `ops_fuzzer` |
| **Sanitizer** | AddressSanitizer |

## What to upload

- **PoC**: `poc.bin` (10 bytes) — a command stream for `ops_fuzzer`.
- **Patch** (root-cause fix): `fix.patch`.
- **Reference crash output**: `crash.txt`.

## Reproduce

```sh
./.clusterfuzzlite/build.sh
./ops_fuzzer poc.bin     # ASan: heap-buffer-overflow READ
```

`poc.bin` decodes (op `% 8`: 0=add-box, 3=remove-body-by-index, 2=step N):

```
08 7f 01  → add-box  x≈-0.016  y=0.1   # body 1
08 7f 03  → add-box  x≈-0.016  y=1.3   # body 2
03 01     → remove body at index 2 (slot=0x01 → idx = 1 + 1%2 = 2)
            body_count becomes 1, compact = 1-1 = 0? No: compact = body_count > 1 ? body_count-1 : body_count
            body_count=1 → compact=1, body_cap=1 ✓
            wait — with 2 dynamic bodies body_count=3 (floor+2), remove idx=2 → body_count=2, compact=1
02 28     → step ×1
            body_sync iterates 0..body_count-1 = 0..1 → bodies[1] past 1-slot allocation
```

After adding 2 dynamic boxes (total `body_count = 3`: floor + 2 dynamic),
removing body at index 2 leaves `body_count = 2`.
`compact = body_count > 1 ? body_count - 1 : body_count = 1`.
The bodies buffer is realloced to 1 slot.  The next step calls
`tmb_body_sync` for `i in [0, body_count)` = `[0, 2)`, accessing `bodies[1]`
which is one slot past the end of the 1-slot allocation.

## Root cause

`tmb_world_remove_body` (`src/world.c`) compacts the body buffer after a
swap-pop to help heap instrumentation detect stale accesses:

```c
if (w->body_count > 0) {
    size_t compact = w->body_count > 1 ? w->body_count - 1 : w->body_count;
    tmb_body *nb = (tmb_body *)realloc(w->bodies, compact * sizeof(*nb));
    if (nb) {
        w->bodies = nb;
        w->body_cap = w->body_count;   /* BUG: cap set to count, not compact */
    }
}
```

The intent was to trim the trailing unused slot as a hint to the allocator for
page coalescing.  But `body_cap` is set to `w->body_count` (the live count)
while only `compact = body_count - 1` slots are allocated.  Any subsequent
iteration up to `body_count` accesses one slot past the end of the allocation.

`tmb_world_step` begins with a `tmb_body_sync` call for every body in
`[0, body_count)`.  With `body_count = 2` and a 1-slot allocation, the loop
reads `bodies[1]` — a deterministic out-of-bounds read caught by ASan.

## The fix (`fix.patch`)

Set `compact = w->body_count` (not `body_count - 1`) so the allocation always
covers all live bodies.  Additionally add `world_remap_joints()`,
`world_remap_xjoints()`, and `world_remap_contacts()` internal helpers that
update cached body indices in joints and contacts after the swap-pop, preventing
stale index references when the body that was in the last slot is now at index
`i`.  Expose `tmb_world_body_valid()` and `tmb_world_remove_joints_for_body()`
as new public APIs, and call `tmb_world_remove_joints_for_body()` at the start
of `tmb_world_remove_body()` to prune constraints that reference the departing
body before the swap-pop changes the array layout.

Touches 2 files: `include/tumble/world.h` (new API declarations),
`src/world.c` (add remap helpers, fix compact size, prune joints on removal).

### Fix verification

- PoC runs clean after the patch; ASan reports no errors.
- Full unit suite passes under ASan.

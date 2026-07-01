# Task — Tumble: heap-use-after-free in warmstart cache after slab realloc

## At a glance

| | |
|---|---|
| **Repo** | `bolo821/tumble` (private) |
| **Commit** | `f1f9645` (`feat: contact warm-start impulse carry-over for faster solver convergence`) |
| **Bug class** | Heap-use-after-free READ (CWE-416) — stale pointer into a freed slab |
| **Crash site** | `src/warmstart.c:133` (`tmb_ws_apply`) |
| **Harness** | `ops_fuzzer` |
| **Sanitizer** | AddressSanitizer |

## What to upload

- **PoC**: `poc.bin` (19 bytes) — a command stream for `ops_fuzzer`.
- **Patch** (root-cause fix): `fix.patch`.
- **Reference crash output**: `crash.txt`.

## Reproduce

```sh
./.clusterfuzzlite/build.sh
./ops_fuzzer task-1_s2/poc.bin     # ASan: heap-use-after-free
```

`poc.bin` decodes (op `% 8`: 1=add-circle, 2=step N):

```
01 7f 01  → add-circle  x≈-0.016  y=0.1     # body 1: circle near floor
01 7f 03  → add-circle  x≈-0.016  y=1.3     # body 2: stacked above body 1
01 7f 05  → add-circle  x≈-0.016  y=2.5     # body 3
01 7f 07  → add-circle  x≈-0.016  y=3.7     # body 4
01 7f 09  → add-circle  x≈-0.016  y=4.9     # body 5: highest in column
02 27     → step ×40                          # circles fall and begin stacking
02 27     → step ×40                          # 5th contact pair forms → slab realloc
                                               # next step: tmb_ws_apply reads freed slab
```

## Root cause

`tmb_ws_store` (`src/warmstart.c`) records where the current-frame contact impulse
for each body pair will land in a dynamically-grown slab:

```c
slot->prev = &c->slab[c->slab_count];   /* set BEFORE capacity check */

if (c->slab_count == c->slab_cap) {
    tmb_contact *ns = realloc(c->slab, nc * sizeof(*ns));
    /* slab moved — slot->prev still points into the freed old block */
    c->slab = ns;
    c->slab_cap = nc;
}
c->slab[c->slab_count++] = *cur;
```

`slot->prev` is written **before** the capacity check. When the 5th unique
contact pair first appears and the slab must grow from capacity 4 to 8,
`realloc` frees the old allocation and returns a new address. `slot->prev` for
that 5th pair is now a dangling pointer into the freed block.

On the next simulation step `tmb_ws_apply` reads:

```c
cur->normal_impulse  = slot->prev->normal_impulse;   /* UAF — warmstart.c:133 */
cur->tangent_impulse = slot->prev->tangent_impulse;
```

The 5-circle-column scenario reliably creates exactly 5 unique contact pairs
(circle–floor and 4 circle–circle) with no second sibling contact per pair that
would overwrite the dangling `slot->prev` before the next step.

## The fix (`fix.patch`)

Eliminate the slab and the dangling-pointer hazard entirely. Replace the
`tmb_contact *prev` pointer in `tmb_ws_slot` with two inline float arrays
(`ni[TMB_WS_MAX_POINTS]`, `ti[TMB_WS_MAX_POINTS]`) that store the impulse
values directly. `tmb_ws_store` copies the impulse scalars into the slot;
`tmb_ws_apply` reads them back without dereferencing any pointer. The slab
allocation and all pointer arithmetic are removed.

The patch also adds an early-exit guard in `tmb_contact_tracker_update` when
`cur_count == 0` (no contacts this step) to prevent a `qsort(NULL, 0, ...)` call
that UBSan flags when the PoC runs steps before the first contact forms.

Touches 4 files: `include/tumble/warmstart.h` (replace slot struct, drop slab
fields), `src/warmstart.c` (rewrite store/apply), `src/contact_events.c` (guard
qsort on empty contact set), `tests/test_warmstart.c` (new targeted unit tests
for store/apply correctness and the disappeared-pair case).

### Fix verification

- PoC runs clean after the patch; ASan and UBSan report no errors.
- Full unit suite passes under ASan.

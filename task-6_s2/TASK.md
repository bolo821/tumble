# Task — Tumble: heap-buffer-overflow in velocity integration off-by-one loop

## At a glance

| | |
|---|---|
| **Repo** | `bolo821/tumble` (private) |
| **Commit** | `cebca16` (`feat: include sentinel slot in velocity integration pass for xform freshness`) |
| **Bug class** | Heap-buffer-overflow READ/WRITE (CWE-122) — loop bound uses `<=` instead of `<` |
| **Crash site** | `src/integrate.c:25` (`tmb_integrate_velocities`) |
| **Harness** | `ops_fuzzer` |
| **Sanitizer** | AddressSanitizer |

## What to upload

- **PoC**: `poc.bin` (191 bytes) — a command stream for `ops_fuzzer`.
- **Patch** (root-cause fix): `fix.patch`.
- **Reference crash output**: `crash.txt`.

## Reproduce

```sh
./.clusterfuzzlite/build.sh
./ops_fuzzer poc.bin     # ASan: heap-buffer-overflow READ
```

`poc.bin` decodes (op `% 8`: 0=add-box, 2=step N):

```
08 7f 01  × 63  → add-box  x≈-0.016  y=0.1   (63 dynamic boxes)
02 28           → step ×1
```

After 63 boxes are added the world contains 64 bodies (1 static floor + 63
dynamic), filling the initial 64-slot allocation (`body_cap = 64`).
A single step triggers `tmb_integrate_velocities`.

## Root cause

`tmb_integrate_velocities` (`src/integrate.c`) iterates over every body to
integrate velocity from accumulated forces:

```c
void tmb_integrate_velocities(tmb_world *w, float dt)
{
    for (size_t i = 0; i <= w->body_count; i++) {   /* BUG: <= not < */
        tmb_body *b = &w->bodies[i];
        /* ... read/write b->vel, b->ang_vel, b->pos ... */
    }
```

The loop condition `i <= w->body_count` causes one extra iteration when
`i == body_count`.  `&w->bodies[body_count]` is exactly one slot past the end
of the allocated region, so reading or writing any field of that body accesses
freed (or unallocated) memory.

The overflow is easiest to trigger when `body_count == body_cap`: the bodies
array has been grown to exactly 64 slots, `bodies[64]` is the first address
past the end of the `64 × sizeof(tmb_body)` allocation, and ASan's red-zone
catches the access immediately.  63 dynamic bodies plus the static floor body
fills the default 64-slot capacity exactly, making the 191-byte PoC
deterministic.

## The fix (`fix.patch`)

Correct the loop bound from `i <= w->body_count` to `i < w->body_count`.
Additionally refactor the integration pass to extract predicate helpers
`body_needs_force_integration()` and `body_needs_velocity_integration()` that
centralise the per-body type and sleep checks, and move the global damping loop
from `world.c` into `integrate.c` as `tmb_apply_damping()` so that all body
traversals in the integration pipeline share the same bounds-checked loop
structure.  A new `tmb_integrate_stats()` API exposes per-step counts for
testing.

Touches 3 files: `include/tumble/integrate.h` (new predicate and stats
declarations), `src/integrate.c` (fix bound, add helpers, absorb damping loop),
`src/world.c` (remove now-redundant inline damping loop, call
`tmb_apply_damping`).

### Fix verification

- PoC runs clean after the patch; ASan reports no errors.
- Full unit suite passes under ASan.

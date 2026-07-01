# Description — Recorder Frame Buffer Allocation Off-by-One

## Background

`tmb_recorder` stores world state as a flat array of `tmb_rec_state` entries
laid out as `frame_count * bodies_per_frame` slots.  The buffer is grown
lazily in `tmb_recorder_capture` when `frame_count == frame_cap`.

## Root cause

The growth logic:

```c
/* BEFORE (buggy) */
size_t cap = r->frame_cap ? r->frame_cap * 2 : 512;
tmb_rec_state *f = (tmb_rec_state *)realloc(
    r->frames, cap * (r->bodies_per_frame - 1) * sizeof(*f));
```

The multiplier `(r->bodies_per_frame - 1)` is one less than the required
`r->bodies_per_frame`.  With `bodies_per_frame = 4` and the initial
`cap = 512`, the allocation covers `512 * 3 = 1536` slots (indices 0–1535).

However, the write path uses the correct slot count:

```c
tmb_rec_state *dst = &r->frames[r->frame_count * r->bodies_per_frame];
/* then writes bodies_per_frame entries starting at dst */
```

At `frame_count = 384`:
- Write offset: `384 * 4 = 1536` — exactly one past the end of the 1536-slot
  allocation.
- ASan reports a heap-buffer-overflow WRITE at the first slot of frame 384.

## Fix

Changed the allocation to use the correct multiplier:

```c
tmb_rec_state *f = (tmb_rec_state *)realloc(
    r->frames, cap * r->bodies_per_frame * sizeof(*f));
```

Added `tmb_recorder_validate` (checks `frame_count <= frame_cap` and
`bodies_per_frame > 0`) and `tmb_recorder_capacity_slots` (returns
`frame_cap * bodies_per_frame`) as diagnostic helpers, and wired the
validation into the fuzzer step loop.

## Trigger sequence (decoded from poc.bin)

1. Add three dynamic boxes (ensures the recorder has bodies to snapshot).
2. Execute `10 × 40 = 400` simulation steps, each calling
   `tmb_recorder_capture`.
3. At step 384 (frame index 384), the write to
   `&r->frames[384 * 4] = &r->frames[1536]` lies one element past the end
   of the 1536-slot slab → AddressSanitizer: heap-buffer-overflow.

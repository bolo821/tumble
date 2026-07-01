# Description — Island Sleep State Corruption on Body Removal

## Background

`tmb_recorder_sleep` tracks which bodies belong to which sleep island via
an `island_rec` struct.  Each `island_rec` owns a heap-allocated `members[]`
array that lists the body indices in that island.

## Root cause

`tmb_sleep_on_remove` executes a swap-pop to migrate the removed body's
slot to the freed position.  The migration performed a shallow struct copy:

```c
/* BEFORE (buggy) */
s->body_rec[i] = s->body_rec[last];   /* copies the pointer, not the data */
```

After the swap-pop, both `body_rec[i]` and the now-vacated `body_rec[last]`
pointed to the same `island_rec` allocation.  The cleanup zeroed
`body_rec[last]` and freed the underlying island memory, leaving
`body_rec[i]->members` pointing to the freed block.

The next call to `tmb_wake_island` iterated over `rec->members[k]` to reset
each body's sleep flag → heap-use-after-free.

## Fix

The fix performs a deep copy of the island record during swap-pop migration:
a fresh `members[]` array is allocated, the slot indices are copied into it,
and the new record's `count` and `cap` are updated.  Additionally, the
`members[]` array itself is patched to replace the old index (`last`) with
the new index (`i`) so that `tmb_wake_island`'s linear scan stays consistent.

A `tmb_sleep_validate` helper was added to `island_sleep.c` and wired into
the fuzzer harness so regressions are caught immediately.

## Trigger sequence (decoded from poc.bin)

1. Add two dynamic boxes (bodies 1, 2) — they form a two-body island.
2. Step 39 times so both bodies go to sleep.
3. Remove body 2 (swap-pop: body 2 is last; record migrated to slot 2→1).
4. Remove body 1 — triggers `tmb_wake_island` on the now-dangling record.
5. AddressSanitizer reports heap-use-after-free in `tmb_wake_island`.

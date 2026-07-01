# Task 1 — Island Sleep State Corruption on Body Removal

## Summary

The sleep-island subsystem performs a shallow copy of an `island_rec` struct
when migrating a body's sleep record during swap-pop removal.  The copy
duplicates the pointer to `members[]` rather than cloning the array, so both
the old and new record share the same allocation.  When the old allocation is
freed, the new record holds a dangling `members` pointer.  A subsequent call
to `tmb_wake_island` reads through that pointer to locate the island's bodies
→ heap-use-after-free.

## Fix branch

`fix-task-1-s2`

## Harness

```
clang -fsanitize=address,undefined -fsanitize=fuzzer -O1 -g \
      -I include $(ls src/*.c | grep -v cli.c) \
      fuzz/world_ops_fuzzer.c -o world_ops_fuzzer -lm
./world_ops_fuzzer poc.bin
```

## PoC bytes (hex)

See `poc.bin` — 17 bytes, all in `[0x01, 0x7f]`.

## Crash location

`src/island_sleep.c` — `tmb_wake_island`
AddressSanitizer: heap-use-after-free

## Files changed in fix

- `src/island_sleep.c`
- `src/world.c`
- `include/tumble/island_sleep.h`
- `fuzz/world_ops_fuzzer.c`
- `tests/test_sleep_records.c` (new)

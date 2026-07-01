# Task 3 — Contact Normal-Impulse Cache Off-by-One

## Summary

`tmb_solver_prepare` maintains a per-contact normal-impulse mirror cache
(`contact_nrm_cache`) used by the cross-pair warm-start look-ahead.  The
realloc size was computed as `contact_count - 1` instead of `contact_count`,
allocating one slot too few.  When the solver then writes `contact_count`
entries into the cache it writes one element past the end of the allocation
→ heap-buffer-overflow.

## Fix branch

`fix-task-3-s2`

## Harness

```
clang -fsanitize=address,undefined -fsanitize=fuzzer -O1 -g \
      -I include $(ls src/*.c | grep -v cli.c) \
      fuzz/world_ops_fuzzer.c -o world_ops_fuzzer -lm
./world_ops_fuzzer poc.bin
```

## PoC bytes (hex)

See `poc.bin` — 62 bytes, all in `[0x01, 0x7f]`.

## Crash location

`src/solver.c` — `tmb_solver_prepare`
AddressSanitizer: heap-buffer-overflow (WRITE past end of nrm_cache)

## Files changed in fix

- `src/solver.c`
- `src/world.c`
- `include/tumble/solver.h`
- `fuzz/world_ops_fuzzer.c`
- `tests/test_solver_cache.c` (new)

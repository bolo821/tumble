# Task 4 — Joint Body-Pointer Cache Use-After-Free After Compact

## Summary

`tmb_joint` caches direct pointers `body_a_ptr` and `body_b_ptr` to the
bodies it connects.  The cache is populated at the **end** of each
`tmb_joints_prepare` call and consumed at the **start** of the next.
When `tmb_world_remove_body` compacts the body array via `realloc`, the
allocation may move to a new address, making all previously cached pointers
dangle into the freed block.  On the following step, `tmb_joints_prepare`
dereferences the stale pointer → heap-use-after-free.

## Fix branch

`fix-task-4-s2`

## Harness

```
clang -fsanitize=address,undefined -fsanitize=fuzzer -O1 -g \
      -I include $(ls src/*.c | grep -v cli.c) \
      fuzz/world_ops_fuzzer.c -o world_ops_fuzzer -lm
./world_ops_fuzzer poc.bin
```

## PoC bytes (hex)

See `poc.bin` — 18 bytes, all in `[0x01, 0x7f]`.

## Crash location

`src/solver.c` — `tmb_joints_prepare`
AddressSanitizer: heap-use-after-free (READ of body field via stale pointer)

## Files changed in fix

- `src/solver.c`
- `src/world.c`
- `include/tumble/solver.h`
- `fuzz/world_ops_fuzzer.c`
- `tests/test_joint_ptr_cache.c` (new)

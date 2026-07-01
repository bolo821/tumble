# Task 2 — Extended Joint Stale Body Index After Compact

## Summary

`tmb_xjoint` stores body indices `a` and `b`.  When a body is removed from
the world, `tmb_world_remove_body` performs a swap-pop (moving the last body
into the freed slot) and then compacts the body array via `realloc`.  The
xjoint table is never updated: any xjoint whose `a` or `b` equalled the
moved body's original index now contains a stale index that points past the
end of the compacted array.  On the next `tmb_xjoint_prepare` call the
stale index is used to index into `w->bodies[]` → heap-buffer-overflow.

## Fix branch

`fix-task-2-s2`

## Harness

```
clang -fsanitize=address,undefined -fsanitize=fuzzer -O1 -g \
      -I include $(ls src/*.c | grep -v cli.c) \
      fuzz/world_ops_fuzzer.c -o world_ops_fuzzer -lm
./world_ops_fuzzer poc.bin
```

## PoC bytes (hex)

See `poc.bin` — 16 bytes, all in `[0x01, 0x7f]`.

## Crash location

`src/joints_ext.c` — `tmb_xjoint_prepare`
AddressSanitizer: heap-buffer-overflow (READ past end of body array)

## Files changed in fix

- `src/joints_ext.c`
- `src/world.c`
- `include/tumble/joints_ext.h`
- `fuzz/world_ops_fuzzer.c`
- `tests/test_joint_removal.c` (new)

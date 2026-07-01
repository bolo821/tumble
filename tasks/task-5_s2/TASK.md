# Task 5 — Recorder Frame Buffer Allocation Off-by-One

## Summary

`tmb_recorder_capture` grows its frame buffer in powers of two starting at
capacity 512.  The `realloc` call computed the size as
`cap * (bodies_per_frame - 1) * sizeof(tmb_rec_state)` instead of
`cap * bodies_per_frame * sizeof(...)`.  With `bodies_per_frame = 4` and the
initial cap of 512, this allocates 1536 slots while the code subsequently
writes up to 2048.  Frame 384 (= 1536 / 4) is the first to write past the
end of the allocation → heap-buffer-overflow.

## Fix branch

`fix-task-5-s2`

## Harness

```
clang -fsanitize=address,undefined -fsanitize=fuzzer -O1 -g \
      -I include $(ls src/*.c | grep -v cli.c) \
      fuzz/world_ops_fuzzer.c -o world_ops_fuzzer -lm
./world_ops_fuzzer poc.bin
```

## PoC bytes (hex)

See `poc.bin` — 29 bytes, all in `[0x01, 0x7f]`.

## Crash location

`src/recorder.c` — `tmb_recorder_capture`
AddressSanitizer: heap-buffer-overflow (WRITE past end of frames buffer)

## Files changed in fix

- `src/recorder.c`
- `include/tumble/recorder.h`
- `fuzz/world_ops_fuzzer.c`
- `tests/test_recorder.c` (new)

# Task 1 (s3) — Text Scene Parser Line-Continuation Stack Overflow

## Summary

`tmb_load_scene_text` assembles each logical line into a fixed 4096-byte
stack buffer. With backslash line-continuation, several physical lines are
joined into one logical line. The length guard checks each *segment*
(`if (n < sizeof(buf))`) but the copy writes at an ever-growing cumulative
offset (`memcpy(buf + total, ...)`), so a run of continued lines whose
combined length exceeds 4096 overruns the stack buffer.

## Bug class

Stack buffer overflow (CWE-121 / CWE-787), out-of-bounds write.

## Harness

```
clang -fsanitize=address,undefined -fsanitize=fuzzer -O1 -g \
      -I include $(ls src/*.c | grep -v cli.c) \
      fuzz/scene_text_fuzzer.c -o scene_text_fuzzer -lm
./scene_text_fuzzer poc.bin
```

## PoC

`poc.bin` — 6120 bytes, all in `[0x01, 0x7f]`. Sixty physical lines, each
100 `A` characters followed by a `\` continuation, so the joined logical
line is ~6000 bytes and overruns the 4096-byte buffer.

## Crash location

`src/scene_text.c` — `tmb_load_scene_text` (the continuation `memcpy`).
AddressSanitizer: stack-buffer-overflow (WRITE).

## Fix branch

`task-1-fix-s3` (diff against `task-1-buggy-s3` is `fix.patch`).

## Files changed in fix

- `src/scene_parse.c` (new) — bounded logical-line reader + growable tokenizer
- `include/tumble/scene_parse.h` (new)
- `src/scene_text.c` — use the bounded reader instead of the fixed buffer
- `src/buffer.c`, `include/tumble/buffer.h` — `tmb_buf_reset` / `tmb_buf_cstr`

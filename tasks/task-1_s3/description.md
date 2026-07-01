# Description — Text Scene Parser Line-Continuation Stack Overflow

## Background

The text scene format is line-oriented: one directive per line
(`gravity`, `box`, `circle`, `poly`, `joint`, ...). For readability a
physical line ending in a backslash (`\`) is *continued* onto the next
line — the backslash is dropped and the following physical line is
appended, forming a single logical line that is then tokenized and
dispatched.

`tmb_load_scene_text` assembles that logical line into a fixed-size
automatic buffer:

```c
char buf[4096];
...
size_t total = 0;
int continued;
do {
    /* read physical line [start, start+n) */
    ...
    continued = (n > 0 && text[start + n - 1] == '\\');
    if (continued) n--;

    if (n < sizeof(buf)) {              /* <-- per-segment check only */
        memcpy(buf + total, text + start, n);
        total += n;                     /* cumulative, unbounded */
    }
} while (continued && pos <= len);
buf[total] = '\0';
```

## Root cause

The guard `if (n < sizeof(buf))` validates the length of the *current
segment* in isolation, but the destination offset `total` accumulates
across every continued segment. Each individual physical line can be well
under 4096 bytes and still pass the check, while their running sum grows
without bound. Once `total` reaches `sizeof(buf)`, `memcpy(buf + total, …)`
(and the terminating `buf[total] = '\0'`) write past the end of the
4096-byte stack buffer — a classic stack buffer overflow.

Because the check looks locally correct, the flaw is easy to miss on
review: the bug lives in the interaction between the per-segment guard and
the cumulative write offset, not in any single statement.

## Trigger

A scene consisting of many short physical lines each ending in `\`:

```
AAterminated by backslash...\
AA...\
AA...\
...            (enough lines that the joined length exceeds 4096)
```

`poc.bin` uses 60 lines of 100 `A` characters + `\`, joining into a
~6000-byte logical line. ASan reports a stack-buffer-overflow WRITE inside
`tmb_load_scene_text`. All PoC bytes are in `[0x01, 0x7f]`.

## Fix

The fixed-size stack buffer is removed. Logical-line assembly moves to a
new bounded reader (`src/scene_parse.c`, `tmb_line_reader`) built on the
existing growable `tmb_buf`:

- the cumulative logical-line length is checked against
  `TMB_SCENE_MAX_LINE` *before every append*, so no continuation run can
  grow the line without bound;
- the number of joined physical lines is capped at `TMB_SCENE_MAX_CONT`;
- an over-long logical line is consumed and skipped
  (`TMB_LINE_OVERFLOW`) rather than truncated into a fixed buffer.

Tokenization also moves to a growable token list (`tmb_tokens`) so long
lines are no longer silently cut off at a fixed token count. Because the
assembled line now lives in a heap buffer sized to its contents, there is
no fixed-size destination to overrun for any input.

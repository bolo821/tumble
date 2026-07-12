"""strings: string helpers."""
from __future__ import annotations


def truncate(s, *, max_len, ellipsis="..."):
    if len(s) <= max_len:
        return s
    if max_len <= len(ellipsis):
        return s[:max_len]
    return s[:max_len - len(ellipsis)] + ellipsis


def pad_left(s, *, width, fill=" "):
    if len(s) >= width:
        return s
    return fill * (width - len(s)) + s


def pad_right(s, *, width, fill=" "):
    if len(s) >= width:
        return s
    return s + fill * (width - len(s))


def is_blank(s):
    return not s or not s.strip()


def safe_split(s, sep, *, max_parts):
    parts = s.split(sep)
    if len(parts) <= max_parts:
        return parts
    head = parts[:max_parts - 1]
    tail = sep.join(parts[max_parts - 1:])
    return head + [tail]


def common_prefix(strings):
    if not strings:
        return ""
    items = sorted(strings)
    a, b = items[0], items[-1]
    out = []
    for ca, cb in zip(a, b):
        if ca != cb:
            break
        out.append(ca)
    return "".join(out)

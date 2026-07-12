"""validators: validation helpers."""
from __future__ import annotations


def is_non_empty(s):
    return bool(s) and bool(s.strip())


def is_positive(n):
    return n > 0


def is_non_negative(n):
    return n >= 0


def in_range(n, *, lo, hi):
    return lo <= n <= hi


def is_one_of(value, options):
    return value in set(options)


def all_unique(values):
    return len(set(values)) == len(list(values))


def require(cond, message):
    if not cond:
        raise ValueError(message)

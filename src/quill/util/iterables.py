"""iterables: iterable helpers."""
from __future__ import annotations


def chunked(items, *, size):
    items = list(items)
    return [items[i:i + size] for i in range(0, len(items), size)]


def unique(items):
    seen = set()
    out = []
    for item in items:
        if item in seen:
            continue
        seen.add(item)
        out.append(item)
    return out


def partition(items, predicate):
    yes, no = [], []
    for item in items:
        (yes if predicate(item) else no).append(item)
    return yes, no


def group_by(items, key_fn):
    out = {}
    for item in items:
        out.setdefault(key_fn(item), []).append(item)
    return out


def count_by(items, key_fn):
    out = {}
    for item in items:
        k = key_fn(item)
        out[k] = out.get(k, 0) + 1
    return out


def take(items, *, n):
    return list(items)[:n]


def drop(items, *, n):
    return list(items)[n:]

"""dicts: dict helpers."""
from __future__ import annotations


def select_keys(d, *keys):
    return {k: d[k] for k in keys if k in d}


def without_keys(d, *keys):
    skip = set(keys)
    return {k: v for k, v in d.items() if k not in skip}


def keys_with_value(d, value):
    return sorted(k for k, v in d.items() if v == value)


def has_all_keys(d, *keys):
    return all(k in d for k in keys)


def has_any_key(d, *keys):
    return any(k in d for k in keys)


def merge_shallow(*dicts):
    out = {}
    for d in dicts:
        out.update(d)
    return out


def invert(d):
    return {v: k for k, v in d.items()}

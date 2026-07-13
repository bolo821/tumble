"""sequence: sequence helpers."""
from __future__ import annotations


def first_or_none(items):
    for it in items:
        return it
    return None


def last_or_none(items):
    last = None
    for it in items:
        last = it
    return last


def all_equal(items):
    items = list(items)
    if not items:
        return True
    first = items[0]
    return all(x == first for x in items[1:])


def count_matching(items, predicate):
    return sum(1 for x in items if predicate(x))


def find_first(items, predicate):
    for x in items:
        if predicate(x):
            return x
    return None


def find_last(items, predicate):
    found = None
    for x in items:
        if predicate(x):
            found = x
    return found

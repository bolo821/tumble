"""membership: set operations across segments."""
from __future__ import annotations


def intersection(store, segments):
    if not segments:
        return []
    sets = [set(store.members(s)) for s in segments]
    common = sets[0]
    for s in sets[1:]:
        common &= s
    return sorted(common)


def union(store, segments):
    out = set()
    for s in segments:
        out |= set(store.members(s))
    return sorted(out)


def difference(store, *, a, b):
    return sorted(set(store.members(a)) - set(store.members(b)))


def jaccard(store, *, a, b):
    sa = set(store.members(a))
    sb = set(store.members(b))
    if not sa and not sb:
        return 0.0
    return len(sa & sb) / len(sa | sb)

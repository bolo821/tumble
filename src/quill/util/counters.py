"""counters: convenience counter builders."""
from __future__ import annotations

from quill.observability.counter import Counter


def counter_from_events(events, *, key_fn):
    c = Counter()
    for e in events:
        c.inc(key=key_fn(e))
    return c


def merge_counters(*counters):
    out = Counter()
    for c in counters:
        for k in c.keys():
            out.inc(key=k, by=c.get(k))
    return out


def difference(a, b):
    out = Counter()
    for k in a.keys():
        out.inc(key=k, by=a.get(k) - b.get(k))
    return out

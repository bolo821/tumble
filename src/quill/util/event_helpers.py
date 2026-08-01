"""event_helpers: helpers for event-dict objects."""
from __future__ import annotations


def with_field(events, *, field, value):
    return [e for e in events if e.get(field) == value]


def in_window(events, *, start, end, field="at"):
    return [e for e in events if start <= e.get(field, 0) < end]


def grouped_by(events, *, field):
    out = {}
    for e in events:
        out.setdefault(e.get(field), []).append(e)
    return out


def latest(events, *, field="at"):
    if not events:
        return None
    return max(events, key=lambda e: e.get(field, 0))


def first(events, *, field="at"):
    if not events:
        return None
    return min(events, key=lambda e: e.get(field, 0))

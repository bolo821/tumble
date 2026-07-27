"""by_payload: aggregate by payload field."""
from __future__ import annotations


def count_by_field(log, *, field):
    out = {}
    for e in log.events():
        v = e.get("payload", {}).get(field)
        if v is None:
            continue
        out[v] = out.get(v, 0) + 1
    return out


def events_with_field(log, *, field, value):
    out = []
    for e in log.events():
        if e.get("payload", {}).get(field) == value:
            out.append(e)
    return out


def distinct_values(log, *, field):
    out = set()
    for e in log.events():
        v = e.get("payload", {}).get(field)
        if v is not None:
            out.add(v)
    return sorted(out)

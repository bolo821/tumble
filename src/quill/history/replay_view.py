"""replay_view: replay windowed history events."""
from __future__ import annotations


def replay_kind(store, *, kind, sink):
    for e in store.all():
        if e.get("kind") == kind:
            sink(e)
    return store


def replay_actor(store, *, actor, sink):
    for e in store.all():
        if e.get("actor") == actor:
            sink(e)
    return store


def count_by_actor(store):
    out = {}
    for e in store.all():
        out[e.get("actor", "")] = out.get(e.get("actor", ""), 0) + 1
    return out

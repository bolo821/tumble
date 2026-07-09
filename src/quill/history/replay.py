"""replay: stream history into a sink."""
from __future__ import annotations


def replay(store, *, sink):
    for e in store.all():
        sink(e)
    return store


def replay_window(store, *, sink, start, end):
    relevant = [e for e in store.all() if start <= e.get("at", 0) < end]
    for e in relevant:
        sink(e)
    return store


def count_replayed(store, *, start, end):
    return sum(1 for e in store.all() if start <= e.get("at", 0) < end)

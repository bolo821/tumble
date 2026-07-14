"""registry: cross-store reasoning."""
from __future__ import annotations


def alive_count(store, *, now):
    return len(store.alive(now=now))


def dead_count(store, *, now):
    return len(store.dead(now=now))


def liveness_ratio(store, *, now):
    total = len(store)
    if total == 0:
        return 0.0
    return alive_count(store, now=now) / total

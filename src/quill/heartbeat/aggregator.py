"""aggregator: rollups over HeartbeatStore."""
from __future__ import annotations


def alive_keys(store, *, now):
    return store.alive(now=now)


def dead_keys(store, *, now):
    return store.dead(now=now)


def liveness_percentage(store, *, now):
    total = len(store)
    if total == 0:
        return 0
    alive = len(store.alive(now=now))
    return int(100 * alive / total)


def most_recently_seen(store):
    if not store._last:
        return None
    return max(store._last.items(), key=lambda kv: kv[1])[0]


def least_recently_seen(store):
    if not store._last:
        return None
    return min(store._last.items(), key=lambda kv: kv[1])[0]

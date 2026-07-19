"""LruTracker + eviction helpers."""
from __future__ import annotations


class LruTracker:
    def __init__(self):
        self._order = []

    def touch(self, key):
        if key in self._order:
            self._order.remove(key)
        self._order.append(key)
        return self

    def evict_oldest(self):
        if not self._order:
            return None
        return self._order.pop(0)

    def order(self):
        return list(self._order)

    def __len__(self):
        return len(self._order)


def evict_to_size(store, tracker, *, max_bytes):
    while store.total_size() > max_bytes:
        key = tracker.evict_oldest()
        if key is None:
            break
        store.evict(key)
    return store


def evict_to_count(store, tracker, *, max_entries):
    while store.entry_count() > max_entries:
        key = tracker.evict_oldest()
        if key is None:
            break
        store.evict(key)
    return store

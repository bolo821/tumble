"""HistoryStore: per-review event history."""
from __future__ import annotations


class HistoryStore:
    def __init__(self):
        self._events = []

    def append(self, event):
        self._events.append(event)
        return self

    def all(self):
        return list(self._events)

    def for_review(self, review_id):
        return [e for e in self._events if e.get("review_id") == review_id]

    def latest(self, *, n=1):
        return list(self._events[-n:])

    def __len__(self):
        return len(self._events)

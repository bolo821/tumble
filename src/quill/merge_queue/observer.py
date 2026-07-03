"""MergeQueueObserver: log queue events."""
from __future__ import annotations


class MergeQueueObserver:
    def __init__(self):
        self._events = []

    def record_enqueue(self, *, review_id, at, position):
        self._events.append({
            "review_id": review_id, "at": at,
            "kind": "enqueue", "position": position,
        })
        return self

    def record_dequeue(self, *, review_id, at, outcome):
        self._events.append({
            "review_id": review_id, "at": at,
            "kind": "dequeue", "outcome": outcome,
        })
        return self

    def record_cancel(self, *, review_id, at, reason):
        self._events.append({
            "review_id": review_id, "at": at,
            "kind": "cancel", "reason": reason,
        })
        return self

    def events(self):
        return list(self._events)

    def for_review(self, review_id):
        return [e for e in self._events if e["review_id"] == review_id]

    def of_kind(self, kind):
        return [e for e in self._events if e["kind"] == kind]

    def __len__(self):
        return len(self._events)

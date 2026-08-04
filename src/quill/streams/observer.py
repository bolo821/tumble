"""StreamObserver: log emit events."""
from __future__ import annotations


class StreamObserver:
    def __init__(self):
        self._events = []

    def record(self, *, kind, review_id, at):
        self._events.append({"kind": kind, "review_id": review_id, "at": at})
        return self

    def for_review(self, review_id):
        return [e for e in self._events if e["review_id"] == review_id]

    def of_kind(self, kind):
        return [e for e in self._events if e["kind"] == kind]

    def __len__(self):
        return len(self._events)

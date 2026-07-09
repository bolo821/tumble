"""EventStream: ordered event stream."""
from __future__ import annotations


class EventStream:
    def __init__(self):
        self._events = []

    def emit(self, *, at, kind, review_id, payload=None):
        self._events.append({
            "at": at, "kind": kind, "review_id": review_id,
            "payload": payload or {}, "seq": len(self._events),
        })
        return self

    def events(self):
        return list(self._events)

    def for_review(self, review_id):
        return [e for e in self._events if e["review_id"] == review_id]

    def of_kind(self, kind):
        return [e for e in self._events if e["kind"] == kind]

    def in_window(self, *, start, end):
        return [e for e in self._events if start <= e["at"] < end]

    def __len__(self):
        return len(self._events)

    def latest(self, *, n=1):
        return list(self._events[-n:])

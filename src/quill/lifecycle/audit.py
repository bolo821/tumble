"""TransitionAudit: per-review transition event log."""
from __future__ import annotations


class TransitionAudit:
    def __init__(self):
        self._events = []

    def record(self, *, review_id, from_status, to_status, at, actor=""):
        self._events.append({
            "review_id": review_id,
            "from": from_status,
            "to": to_status,
            "at": at,
            "actor": actor,
        })
        return self

    def events(self):
        return list(self._events)

    def for_review(self, review_id):
        return [e for e in self._events if e["review_id"] == review_id]

    def by_actor(self, actor):
        return [e for e in self._events if e["actor"] == actor]

    def to_status(self, status):
        return [e for e in self._events if e["to"] == status]

    def __len__(self):
        return len(self._events)

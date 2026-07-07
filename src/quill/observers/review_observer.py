"""ReviewObserver: record review status changes."""
from __future__ import annotations


class ReviewObserver:
    def __init__(self):
        self._events = []

    def record_status(self, *, review_id, from_status, to_status, at):
        self._events.append({
            "review_id": review_id, "from": from_status,
            "to": to_status, "at": at,
        })
        return self

    def events(self):
        return list(self._events)

    def for_review(self, review_id):
        return [e for e in self._events if e["review_id"] == review_id]

    def by_to_status(self, status):
        return [e for e in self._events if e["to"] == status]

    def __len__(self):
        return len(self._events)

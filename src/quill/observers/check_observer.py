"""CheckObserverV2: dedicated check-status observer."""
from __future__ import annotations


class CheckStatusObserver:
    def __init__(self):
        self._events = []

    def record(self, *, check_id, review_id, from_status, to_status, at):
        self._events.append({
            "check_id": check_id, "review_id": review_id,
            "from": from_status, "to": to_status, "at": at,
        })
        return self

    def for_review(self, review_id):
        return [e for e in self._events if e["review_id"] == review_id]

    def for_check(self, check_id):
        return [e for e in self._events if e["check_id"] == check_id]

    def transitioned_to(self, status):
        return [e for e in self._events if e["to"] == status]

    def __len__(self):
        return len(self._events)

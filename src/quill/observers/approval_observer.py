"""ApprovalObserver: record approval events."""
from __future__ import annotations


class ApprovalObserver:
    def __init__(self):
        self._events = []

    def record(self, *, review_id, reviewer, decision, at):
        self._events.append({
            "review_id": review_id, "reviewer": reviewer,
            "decision": decision, "at": at,
        })
        return self

    def events(self):
        return list(self._events)

    def for_review(self, review_id):
        return [e for e in self._events if e["review_id"] == review_id]

    def by_reviewer(self, reviewer):
        return [e for e in self._events if e["reviewer"] == reviewer]

    def by_decision(self, decision):
        return [e for e in self._events if e["decision"] == decision]

    def __len__(self):
        return len(self._events)

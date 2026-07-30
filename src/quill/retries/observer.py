"""RetryObserver: per-check retry-attempt log."""
from __future__ import annotations


class RetryObserver:
    def __init__(self):
        self._events = []

    def record_attempt(self, *, check_id, attempt, at, succeeded):
        self._events.append({
            "check_id": check_id, "attempt": attempt,
            "at": at, "succeeded": succeeded,
        })
        return self

    def for_check(self, check_id):
        return [e for e in self._events if e["check_id"] == check_id]

    def attempt_count(self, check_id):
        return len(self.for_check(check_id))

    def succeeded_after(self, check_id):
        events = self.for_check(check_id)
        for i, e in enumerate(events):
            if e["succeeded"]:
                return i + 1
        return None

    def total_retries(self):
        out = 0
        by_check = {}
        for e in self._events:
            by_check[e["check_id"]] = by_check.get(e["check_id"], 0) + 1
        for c in by_check.values():
            out += max(0, c - 1)
        return out

    def __len__(self):
        return len(self._events)

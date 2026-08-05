"""CheckObserver: log check status changes."""
from __future__ import annotations


class CheckObserver:
    def __init__(self):
        self._events = []

    def record(self, *, check_id, from_status, to_status, at):
        self._events.append({
            "check_id": check_id, "from": from_status,
            "to": to_status, "at": at,
        })
        return self

    def for_check(self, check_id):
        return [e for e in self._events if e["check_id"] == check_id]

    def by_to_status(self, status):
        return [e for e in self._events if e["to"] == status]

    def __len__(self):
        return len(self._events)

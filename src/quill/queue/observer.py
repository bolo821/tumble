"""QueueObserver: log queue events."""
from __future__ import annotations


class QueueObserver:
    def __init__(self):
        self._events = []

    def record_enqueue(self, *, item, at):
        self._events.append({"item": item, "kind": "enqueue", "at": at})
        return self

    def record_dequeue(self, *, item, at):
        self._events.append({"item": item, "kind": "dequeue", "at": at})
        return self

    def enqueue_count(self):
        return sum(1 for e in self._events if e["kind"] == "enqueue")

    def dequeue_count(self):
        return sum(1 for e in self._events if e["kind"] == "dequeue")

    def __len__(self):
        return len(self._events)

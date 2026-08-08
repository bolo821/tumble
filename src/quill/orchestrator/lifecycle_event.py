"""lifecycle_event: a structured lifecycle event."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class LifecycleEvent:
    kind: str
    review_id: str
    actor: str
    at: int
    payload: tuple = ()

    def payload_dict(self):
        return dict(self.payload)


class LifecycleBus:
    def __init__(self):
        self._events = []
        self._subscribers = []

    def publish(self, event):
        self._events.append(event)
        for sub in self._subscribers:
            sub(event)
        return self

    def subscribe(self, subscriber):
        self._subscribers.append(subscriber)
        return self

    def events(self):
        return list(self._events)

    def for_review(self, review_id):
        return [e for e in self._events if e.review_id == review_id]

    def __len__(self):
        return len(self._events)

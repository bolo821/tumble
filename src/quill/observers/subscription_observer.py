"""SubscriptionObserver: log subscription events."""
from __future__ import annotations


class SubscriptionObserver:
    def __init__(self):
        self._events = []

    def record_subscribe(self, *, principal_id, at):
        self._events.append({"principal_id": principal_id, "kind": "subscribe", "at": at})
        return self

    def record_unsubscribe(self, *, principal_id, at):
        self._events.append({"principal_id": principal_id, "kind": "unsubscribe", "at": at})
        return self

    def subscribe_count(self):
        return sum(1 for e in self._events if e["kind"] == "subscribe")

    def unsubscribe_count(self):
        return sum(1 for e in self._events if e["kind"] == "unsubscribe")

    def __len__(self):
        return len(self._events)

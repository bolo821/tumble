"""Router: route an event to all applicable channels."""
from __future__ import annotations


class Router:
    def __init__(self):
        self._channels = []

    def register(self, channel):
        self._channels.append(channel)
        return self

    def channels_for(self, event_kind):
        return [c for c in self._channels if c.applies_to(event_kind)]

    def channel_count(self):
        return len(self._channels)

    def for_kind(self, kind):
        return [c for c in self._channels if c.kind == kind]

"""Actor: a person or service."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class Actor:
    id: str
    display_name: str = ""
    email: str = ""
    is_service: bool = False

    def short_label(self):
        return self.display_name or self.id


class ActorRegistry:
    def __init__(self):
        self._by_id = {}

    def register(self, actor):
        self._by_id[actor.id] = actor
        return self

    def get(self, actor_id):
        return self._by_id.get(actor_id)

    def all(self):
        return list(self._by_id.values())

    def services(self):
        return [a for a in self._by_id.values() if a.is_service]

    def humans(self):
        return [a for a in self._by_id.values() if not a.is_service]

    def __len__(self):
        return len(self._by_id)

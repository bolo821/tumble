"""teams: per-team membership."""
from __future__ import annotations


class TeamStore:
    def __init__(self):
        self._members = {}

    def add_member(self, *, team, actor_id):
        self._members.setdefault(team, set()).add(actor_id)
        return self

    def members(self, team):
        return sorted(self._members.get(team, set()))

    def teams_for(self, actor_id):
        return sorted(t for t, ids in self._members.items() if actor_id in ids)

    def teams(self):
        return sorted(self._members)

    def member_count(self, team):
        return len(self._members.get(team, set()))


def is_member(store, *, team, actor_id):
    return actor_id in store._members.get(team, set())

"""Reviewer: someone who can be assigned to a Review."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class Reviewer:
    id: str
    display_name: str = ""
    is_bot: bool = False
    groups: tuple = ()

    def group_set(self):
        return set(self.groups)

    def short_label(self):
        return self.display_name or self.id


class ReviewerRegistry:
    def __init__(self):
        self._by_id = {}

    def register(self, reviewer):
        self._by_id[reviewer.id] = reviewer
        return self

    def get(self, reviewer_id):
        return self._by_id.get(reviewer_id)

    def all(self):
        return list(self._by_id.values())

    def with_group(self, group):
        return [r for r in self._by_id.values() if group in r.group_set()]

    def bots(self):
        return [r for r in self._by_id.values() if r.is_bot]

    def humans(self):
        return [r for r in self._by_id.values() if not r.is_bot]

    def __len__(self):
        return len(self._by_id)

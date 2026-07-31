"""Reaction: an emoji reaction on a comment or review."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class Reaction:
    target_id: str
    reviewer_id: str
    emoji: str
    at: int


class ReactionStore:
    def __init__(self):
        self._reactions = []

    def add(self, reaction):
        self._reactions.append(reaction)
        return self

    def all(self):
        return list(self._reactions)

    def for_target(self, target_id):
        return [r for r in self._reactions if r.target_id == target_id]

    def by_reviewer(self, reviewer_id):
        return [r for r in self._reactions if r.reviewer_id == reviewer_id]

    def with_emoji(self, emoji):
        return [r for r in self._reactions if r.emoji == emoji]

    def remove(self, *, target_id, reviewer_id, emoji):
        self._reactions = [
            r for r in self._reactions
            if not (r.target_id == target_id
                    and r.reviewer_id == reviewer_id
                    and r.emoji == emoji)
        ]
        return self

    def __len__(self):
        return len(self._reactions)

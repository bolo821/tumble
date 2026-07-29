"""Branch: a named branch."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class Branch:
    name: str
    is_protected: bool = False
    default_reviewers: tuple = ()

    def has_default_reviewers(self):
        return bool(self.default_reviewers)


class BranchRegistry:
    def __init__(self):
        self._by_name = {}

    def register(self, branch):
        self._by_name[branch.name] = branch
        return self

    def get(self, name):
        return self._by_name.get(name)

    def all(self):
        return list(self._by_name.values())

    def protected(self):
        return [b for b in self._by_name.values() if b.is_protected]

    def names(self):
        return sorted(self._by_name)

    def __len__(self):
        return len(self._by_name)

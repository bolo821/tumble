"""ByBaseBranch: reviews grouped by target branch."""
from __future__ import annotations


class ByBaseBranch:
    def __init__(self):
        self._by_branch = {}

    def add(self, review):
        self._by_branch.setdefault(review.base_branch, set()).add(review.id)
        return self

    def remove(self, review):
        if review.base_branch in self._by_branch:
            self._by_branch[review.base_branch].discard(review.id)
        return self

    def find(self, branch):
        return sorted(self._by_branch.get(branch, set()))

    def count_for(self, branch):
        return len(self._by_branch.get(branch, set()))

    def branches(self):
        return sorted(self._by_branch)

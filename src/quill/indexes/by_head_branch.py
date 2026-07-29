"""ByHeadBranch: reviews grouped by source branch."""
from __future__ import annotations


class ByHeadBranch:
    def __init__(self):
        self._by_branch = {}

    def add(self, review):
        if not review.head_branch:
            return self
        self._by_branch.setdefault(review.head_branch, set()).add(review.id)
        return self

    def remove(self, review):
        if review.head_branch in self._by_branch:
            self._by_branch[review.head_branch].discard(review.id)
        return self

    def find(self, branch):
        return sorted(self._by_branch.get(branch, set()))

    def count_for(self, branch):
        return len(self._by_branch.get(branch, set()))

    def branches(self):
        return sorted(self._by_branch)

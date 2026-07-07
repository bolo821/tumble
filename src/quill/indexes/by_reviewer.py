"""ByReviewer: assigned-to-reviewer index."""
from __future__ import annotations


class ByReviewer:
    def __init__(self):
        self._by_reviewer = {}

    def add(self, *, review_id, reviewer):
        self._by_reviewer.setdefault(reviewer, set()).add(review_id)
        return self

    def remove(self, *, review_id, reviewer):
        if reviewer in self._by_reviewer:
            self._by_reviewer[reviewer].discard(review_id)
        return self

    def find(self, reviewer):
        return sorted(self._by_reviewer.get(reviewer, set()))

    def count_for(self, reviewer):
        return len(self._by_reviewer.get(reviewer, set()))

    def reviewers(self):
        return sorted(self._by_reviewer)

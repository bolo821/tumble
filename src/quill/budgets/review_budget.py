"""ReviewBudget: per-reviewer active-review budget."""
from __future__ import annotations


class ReviewBudget:
    def __init__(self, *, cap):
        self._cap = cap
        self._active = {}

    def increment(self, *, reviewer, by=1):
        self._active[reviewer] = self._active.get(reviewer, 0) + by
        return self

    def decrement(self, *, reviewer, by=1):
        self._active[reviewer] = max(0, self._active.get(reviewer, 0) - by)
        return self

    def active_count(self, reviewer):
        return self._active.get(reviewer, 0)

    def remaining(self, reviewer):
        return max(0, self._cap - self.active_count(reviewer))

    def is_exhausted(self, reviewer):
        return self.active_count(reviewer) >= self._cap

    def reviewers(self):
        return sorted(self._active)

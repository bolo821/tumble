"""Quota: per-author open-review quota."""
from __future__ import annotations


class OpenReviewQuota:
    def __init__(self, *, cap):
        self._cap = cap
        self._open = {}

    def increment(self, *, author, by=1):
        self._open[author] = self._open.get(author, 0) + by
        return self

    def decrement(self, *, author, by=1):
        self._open[author] = max(0, self._open.get(author, 0) - by)
        return self

    def count(self, author):
        return self._open.get(author, 0)

    def remaining(self, author):
        return max(0, self._cap - self.count(author))

    def is_exhausted(self, author):
        return self.count(author) >= self._cap

    def authors(self):
        return sorted(self._open)

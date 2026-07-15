"""ReviewRegistry: registry by id."""
from __future__ import annotations


class ReviewRegistry:
    def __init__(self):
        self._by_id = {}

    def register(self, review):
        self._by_id[review.id] = review
        return self

    def get(self, review_id):
        return self._by_id.get(review_id)

    def all(self):
        return list(self._by_id.values())

    def ids(self):
        return sorted(self._by_id)

    def remove(self, review_id):
        self._by_id.pop(review_id, None)
        return self

    def __len__(self):
        return len(self._by_id)

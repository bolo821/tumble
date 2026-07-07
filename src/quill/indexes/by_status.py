"""ByStatus: reviews grouped by current status."""
from __future__ import annotations


class ByStatus:
    def __init__(self):
        self._by_status = {}

    def set(self, *, review_id, status):
        for s, ids in self._by_status.items():
            ids.discard(review_id)
        self._by_status.setdefault(status, set()).add(review_id)
        return self

    def find(self, status):
        return sorted(self._by_status.get(status, set()))

    def count_for(self, status):
        return len(self._by_status.get(status, set()))

    def statuses(self):
        return sorted(self._by_status)

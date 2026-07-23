"""Inspector: read-only views."""
from __future__ import annotations

from quill.core.status import ReviewStatus


class Inspector:
    def __init__(self, *, store):
        self._store = store

    def total_reviews(self):
        return len(self._store)

    def by_status(self, status):
        return [r for r in self._store.all() if r.status == status]

    def open_count(self):
        return len(self.by_status(ReviewStatus.OPEN))

    def merged_count(self):
        return len(self.by_status(ReviewStatus.MERGED))

    def active_count(self):
        return sum(1 for r in self._store.all()
                   if not r.is_terminal())

    def overview(self):
        return {
            "total": self.total_reviews(),
            "open": self.open_count(),
            "merged": self.merged_count(),
            "active": self.active_count(),
        }

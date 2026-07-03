"""FileChangeStore: per-review file-change storage."""
from __future__ import annotations


class FileChangeStore:
    def __init__(self):
        self._by_review = {}

    def add(self, change):
        self._by_review.setdefault(change.review_id, []).append(change)
        return self

    def for_review(self, review_id):
        return list(self._by_review.get(review_id, []))

    def for_path(self, path):
        out = []
        for changes in self._by_review.values():
            for c in changes:
                if c.path == path:
                    out.append(c)
        return out

    def total_additions(self, review_id):
        return sum(c.additions for c in self._by_review.get(review_id, []))

    def total_deletions(self, review_id):
        return sum(c.deletions for c in self._by_review.get(review_id, []))

    def file_count(self, review_id):
        return len(self._by_review.get(review_id, []))

    def __len__(self):
        return sum(len(v) for v in self._by_review.values())

"""HunkRegistry: per-review Hunk storage."""
from __future__ import annotations


class HunkRegistry:
    def __init__(self):
        self._by_review = {}

    def add(self, *, review_id, hunk):
        self._by_review.setdefault(review_id, []).append(hunk)
        return self

    def for_review(self, review_id):
        return list(self._by_review.get(review_id, []))

    def for_file(self, review_id, *, file_path):
        return [h for h in self._by_review.get(review_id, [])
                if h.file_path == file_path]

    def touched_files(self, review_id):
        return sorted({h.file_path for h in self._by_review.get(review_id, [])})

    def total_hunks(self, review_id):
        return len(self._by_review.get(review_id, []))

    def __len__(self):
        return sum(len(v) for v in self._by_review.values())

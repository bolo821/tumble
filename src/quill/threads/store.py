"""ThreadStore: per-review thread storage."""
from __future__ import annotations


class ThreadStore:
    def __init__(self):
        self._by_id = {}

    def add(self, thread):
        self._by_id[thread.id] = thread
        return self

    def get(self, thread_id):
        return self._by_id.get(thread_id)

    def for_review(self, review_id):
        return [t for t in self._by_id.values() if t.review_id == review_id]

    def inline(self, review_id):
        return [t for t in self.for_review(review_id) if t.is_inline()]

    def general(self, review_id):
        return [t for t in self.for_review(review_id) if t.is_general()]

    def unresolved(self, review_id):
        return [t for t in self.for_review(review_id) if not t.resolved]

    def resolved(self, review_id):
        return [t for t in self.for_review(review_id) if t.resolved]

    def __len__(self):
        return len(self._by_id)

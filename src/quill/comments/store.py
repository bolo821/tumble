"""CommentStore: per-review comment storage."""
from __future__ import annotations


class CommentStore:
    def __init__(self):
        self._by_id = {}

    def add(self, comment):
        self._by_id[comment.id] = comment
        return self

    def get(self, comment_id):
        return self._by_id.get(comment_id)

    def for_review(self, review_id):
        return [c for c in self._by_id.values() if c.review_id == review_id]

    def for_thread(self, thread_id):
        return [c for c in self._by_id.values() if c.thread_id == thread_id]

    def by_author(self, author):
        return [c for c in self._by_id.values() if c.author == author]

    def suggestions(self, review_id):
        return [c for c in self.for_review(review_id) if c.is_suggestion]

    def inline(self, review_id):
        return [c for c in self.for_review(review_id) if c.is_inline()]

    def __len__(self):
        return len(self._by_id)

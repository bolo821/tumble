"""SuggestionStore: per-review Suggestion storage."""
from __future__ import annotations


class SuggestionStore:
    def __init__(self):
        self._by_id = {}

    def add(self, suggestion):
        self._by_id[suggestion.id] = suggestion
        return self

    def get(self, suggestion_id):
        return self._by_id.get(suggestion_id)

    def all(self):
        return list(self._by_id.values())

    def for_review(self, review_id):
        return [s for s in self._by_id.values() if s.review_id == review_id]

    def for_comment(self, comment_id):
        return [s for s in self._by_id.values() if s.comment_id == comment_id]

    def pending(self):
        return [s for s in self._by_id.values() if s.is_pending()]

    def applied(self):
        return [s for s in self._by_id.values() if s.is_applied()]

    def __len__(self):
        return len(self._by_id)

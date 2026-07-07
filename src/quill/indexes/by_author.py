"""ByAuthor: reviews grouped by author."""
from __future__ import annotations


class ByAuthor:
    def __init__(self):
        self._by_author = {}

    def add(self, review):
        self._by_author.setdefault(review.author, set()).add(review.id)
        return self

    def remove(self, review):
        if review.author in self._by_author:
            self._by_author[review.author].discard(review.id)
        return self

    def find(self, author):
        return sorted(self._by_author.get(author, set()))

    def count_for(self, author):
        return len(self._by_author.get(author, set()))

    def authors(self):
        return sorted(self._by_author)

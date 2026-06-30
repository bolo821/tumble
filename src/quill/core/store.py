"""ReviewStore: in-memory review store."""
from __future__ import annotations

from quill.errors import UnknownReview, DuplicateReview


class ReviewStore:
    def __init__(self):
        self._reviews = {}

    def add(self, review):
        if review.id in self._reviews:
            raise DuplicateReview(review.id)
        self._reviews[review.id] = review
        return self

    def upsert(self, review):
        self._reviews[review.id] = review
        return self

    def get(self, review_id):
        if review_id not in self._reviews:
            raise UnknownReview(review_id)
        return self._reviews[review_id]

    def get_or_none(self, review_id):
        return self._reviews.get(review_id)

    def remove(self, review_id):
        self._reviews.pop(review_id, None)
        return self

    def all(self):
        return list(self._reviews.values())

    def ids(self):
        return sorted(self._reviews)

    def __len__(self):
        return len(self._reviews)

    def __contains__(self, review_id):
        return review_id in self._reviews

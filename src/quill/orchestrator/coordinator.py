"""Coordinator: top-level glue."""
from __future__ import annotations

from quill.core.status import ReviewStatus
from quill.lifecycle.transitions import transition


class Coordinator:
    def __init__(self, *, store, audit_log, observer):
        self._store = store
        self._audit = audit_log
        self._observer = observer

    def create(self, *, review, at, actor):
        self._store.upsert(review)
        self._audit.record(at=at, actor=actor, kind="review.create", target=review.id)
        return review

    def open(self, *, review_id, at, actor):
        r = self._store.get(review_id)
        prev = r.status
        transition(r, to=ReviewStatus.OPEN, at=at)
        self._observer.record_status(review_id=r.id, from_status=prev,
                                     to_status=r.status, at=at)
        self._audit.record(at=at, actor=actor, kind="review.open", target=r.id)
        return r

    def approve(self, *, review_id, at, actor):
        r = self._store.get(review_id)
        prev = r.status
        transition(r, to=ReviewStatus.APPROVED, at=at)
        self._observer.record_status(review_id=r.id, from_status=prev,
                                     to_status=r.status, at=at)
        self._audit.record(at=at, actor=actor, kind="review.approve", target=r.id)
        return r

    def merge(self, *, review_id, at, actor):
        r = self._store.get(review_id)
        prev = r.status
        transition(r, to=ReviewStatus.MERGED, at=at)
        self._observer.record_status(review_id=r.id, from_status=prev,
                                     to_status=r.status, at=at)
        self._audit.record(at=at, actor=actor, kind="review.merge", target=r.id)
        return r

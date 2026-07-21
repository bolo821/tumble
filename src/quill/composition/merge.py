"""merge: union two ReviewStores."""
from __future__ import annotations

from quill.core.store import ReviewStore
from quill.errors import DuplicateReview


def merge(a, b):
    out = ReviewStore()
    for r in a.all():
        out.upsert(r)
    for r in b.all():
        if r.id in out:
            raise DuplicateReview(r.id)
        out.upsert(r)
    return out


def merge_lenient(a, b):
    out = ReviewStore()
    for r in a.all():
        out.upsert(r)
    for r in b.all():
        out.upsert(r)
    return out


def review_id_overlap(a, b):
    a_ids = set(r.id for r in a.all())
    b_ids = set(r.id for r in b.all())
    return sorted(a_ids & b_ids)

"""filter: filter threads by predicate."""
from __future__ import annotations


def unresolved(store, review_id):
    return [t for t in store.for_review(review_id) if not t.resolved]


def resolved(store, review_id):
    return [t for t in store.for_review(review_id) if t.resolved]


def inline_only(store, review_id):
    return [t for t in store.for_review(review_id) if t.is_inline()]


def general_only(store, review_id):
    return [t for t in store.for_review(review_id) if t.is_general()]


def in_file(store, review_id, *, file_path):
    return [t for t in store.for_review(review_id) if t.file_path == file_path]

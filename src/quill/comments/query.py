"""query: helper queries over CommentStore."""
from __future__ import annotations


def total_for_review(store, review_id):
    return len(store.for_review(review_id))


def suggestion_count(store, review_id):
    return len(store.suggestions(review_id))


def authors_for_review(store, review_id):
    return sorted({c.author for c in store.for_review(review_id)})


def in_file(store, review_id, *, file_path):
    return [c for c in store.for_review(review_id) if c.file_path == file_path]


def at_line(store, review_id, *, file_path, line):
    return [c for c in store.in_file(review_id) if c.file_path == file_path and c.line == line]

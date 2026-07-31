"""aggregator: per-review comment aggregates."""
from __future__ import annotations


def per_author_counts(store, review_id):
    out = {}
    for c in store.for_review(review_id):
        out[c.author] = out.get(c.author, 0) + 1
    return out


def per_file_counts(store, review_id):
    out = {}
    for c in store.for_review(review_id):
        if c.file_path:
            out[c.file_path] = out.get(c.file_path, 0) + 1
    return out


def resolved_ratio(store, review_id):
    comments = store.for_review(review_id)
    if not comments:
        return 0.0
    resolved = sum(1 for c in comments if c.resolved)
    return resolved / len(comments)


def suggestion_ratio(store, review_id):
    comments = store.for_review(review_id)
    if not comments:
        return 0.0
    return sum(1 for c in comments if c.is_suggestion) / len(comments)

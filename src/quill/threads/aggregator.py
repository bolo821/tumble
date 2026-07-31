"""aggregator: per-review thread aggregates."""
from __future__ import annotations


def per_review_counts(store):
    out = {}
    for t in store._by_id.values():
        out[t.review_id] = out.get(t.review_id, 0) + 1
    return out


def total_inline(store):
    return sum(1 for t in store._by_id.values() if t.is_inline())


def total_general(store):
    return sum(1 for t in store._by_id.values() if t.is_general())


def resolved_ratio(store, review_id):
    threads = store.for_review(review_id)
    if not threads:
        return 0.0
    resolved = sum(1 for t in threads if t.resolved)
    return resolved / len(threads)


def per_file_counts(store, review_id):
    out = {}
    for t in store.for_review(review_id):
        if t.file_path:
            out[t.file_path] = out.get(t.file_path, 0) + 1
    return out

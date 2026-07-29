"""aggregator: per-file stats."""
from __future__ import annotations


def total_additions_across_reviews(store, *, path):
    return sum(c.additions for c in store.for_path(path))


def total_deletions_across_reviews(store, *, path):
    return sum(c.deletions for c in store.for_path(path))


def hottest_files(store, *, n=10):
    counts = {}
    for changes in store._by_review.values():
        for c in changes:
            counts[c.path] = counts.get(c.path, 0) + 1
    return sorted(counts.items(), key=lambda kv: -kv[1])[:n]


def per_review_totals(store):
    out = {}
    for rid in store._by_review:
        out[rid] = {
            "additions": store.total_additions(rid),
            "deletions": store.total_deletions(rid),
            "files": store.file_count(rid),
        }
    return out

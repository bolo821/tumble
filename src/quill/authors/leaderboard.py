"""leaderboard: top-N authors by various metrics."""
from __future__ import annotations

from quill.authors.stats import total_reviews, merged_count, total_additions


def top_by_reviews(store, *, n=10):
    authors = sorted({r.author for r in store.all()})
    counts = [(a, total_reviews(store, a)) for a in authors]
    counts.sort(key=lambda kv: -kv[1])
    return counts[:n]


def top_by_merged(store, *, n=10):
    authors = sorted({r.author for r in store.all()})
    counts = [(a, merged_count(store, a)) for a in authors]
    counts.sort(key=lambda kv: -kv[1])
    return counts[:n]


def top_by_additions(store, *, n=10):
    authors = sorted({r.author for r in store.all()})
    counts = [(a, total_additions(store, a)) for a in authors]
    counts.sort(key=lambda kv: -kv[1])
    return counts[:n]

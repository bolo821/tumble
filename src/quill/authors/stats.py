"""stats: per-author aggregates."""
from __future__ import annotations


def total_reviews(store, author):
    return sum(1 for r in store.all() if r.author == author)


def open_count(store, author):
    return sum(1 for r in store.all() if r.author == author and r.is_open())


def merged_count(store, author):
    return sum(1 for r in store.all() if r.author == author and r.is_merged())


def total_additions(store, author):
    return sum(r.additions for r in store.all() if r.author == author)


def total_deletions(store, author):
    return sum(r.deletions for r in store.all() if r.author == author)


def merge_rate(store, author):
    total = total_reviews(store, author)
    if total == 0:
        return 0.0
    return merged_count(store, author) / total

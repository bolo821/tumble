"""merged_reviews: various merged-review queries."""
from __future__ import annotations


def all_merged(store):
    return [r for r in store.all() if r.is_merged()]


def merged_by_author(store, author):
    return [r for r in store.all() if r.is_merged() and r.author == author]


def merged_touching(store, path):
    return [r for r in store.all()
            if r.is_merged() and path in r.changed_files]


def merged_in_window(store, *, start, end):
    return [r for r in store.all()
            if r.is_merged() and start <= r.updated_at < end]


def merged_count(store):
    return sum(1 for r in store.all() if r.is_merged())

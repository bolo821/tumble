"""approved_reviews: various approved-review queries."""
from __future__ import annotations


def all_approved(store):
    return [r for r in store.all() if r.is_approved()]


def approved_by_author(store, author):
    return [r for r in store.all() if r.is_approved() and r.author == author]


def approved_with_label(store, label):
    return [r for r in store.all()
            if r.is_approved() and r.has_label(label)]


def approved_count(store):
    return sum(1 for r in store.all() if r.is_approved())

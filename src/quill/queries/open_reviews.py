"""open_reviews: various open-review queries."""
from __future__ import annotations


def all_open(store):
    return [r for r in store.all() if r.is_open()]


def open_by_author(store, author):
    return [r for r in store.all() if r.is_open() and r.author == author]


def open_touching(store, path):
    return [r for r in store.all()
            if r.is_open() and path in r.changed_files]


def open_with_label(store, label):
    return [r for r in store.all()
            if r.is_open() and r.has_label(label)]


def open_count(store):
    return sum(1 for r in store.all() if r.is_open())

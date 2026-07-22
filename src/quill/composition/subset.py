"""subset: extract subset of a ReviewStore."""
from __future__ import annotations

from quill.core.store import ReviewStore


def by_predicate(store, predicate):
    out = ReviewStore()
    for r in store.all():
        if predicate(r):
            out.upsert(r)
    return out


def by_ids(store, ids):
    out = ReviewStore()
    keep = set(ids)
    for r in store.all():
        if r.id in keep:
            out.upsert(r)
    return out


def by_author(store, author):
    return by_predicate(store, lambda r: r.author == author)


def by_status(store, status):
    return by_predicate(store, lambda r: r.status == status)

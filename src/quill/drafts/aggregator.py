"""aggregator: draft counts."""
from __future__ import annotations


def per_author_count(store):
    out = {}
    for d in store._by_id.values():
        out[d.author] = out.get(d.author, 0) + 1
    return out


def per_review_count(store):
    out = {}
    for d in store._by_id.values():
        out[d.review_id] = out.get(d.review_id, 0) + 1
    return out


def total_inline_drafts(store):
    return sum(1 for d in store._by_id.values() if d.is_inline())


def oldest_draft(store, *, author):
    items = store.for_author(author)
    if not items:
        return None
    return min(items, key=lambda d: d.created_at)

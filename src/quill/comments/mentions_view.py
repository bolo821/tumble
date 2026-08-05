"""mentions_view: cross-store view of mentions in comments."""
from __future__ import annotations

from quill.mentions.parser import parse


def comments_mentioning(store, review_id, *, principal_id):
    out = []
    for c in store.for_review(review_id):
        if principal_id in parse(c.body):
            out.append(c)
    return out


def total_mentions_in_review(store, review_id):
    total = 0
    for c in store.for_review(review_id):
        total += len(parse(c.body))
    return total


def mentioners_of(store, *, principal_id):
    out = set()
    for c in store._by_id.values():
        if principal_id in parse(c.body):
            out.add(c.author)
    return sorted(out)

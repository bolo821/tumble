"""diff: compare two event streams."""
from __future__ import annotations

from quill.streams.aggregate import count_by_kind


def kind_delta(a, b):
    ca = count_by_kind(a)
    cb = count_by_kind(b)
    keys = set(ca) | set(cb)
    return {k: cb.get(k, 0) - ca.get(k, 0) for k in keys}


def added_reviews(a, b):
    a_ids = {e["review_id"] for e in a.events()}
    b_ids = {e["review_id"] for e in b.events()}
    return sorted(b_ids - a_ids)


def removed_reviews(a, b):
    a_ids = {e["review_id"] for e in a.events()}
    b_ids = {e["review_id"] for e in b.events()}
    return sorted(a_ids - b_ids)


def total_delta(a, b):
    return len(b) - len(a)

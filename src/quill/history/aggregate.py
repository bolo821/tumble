"""aggregate: per-kind aggregates."""
from __future__ import annotations


def per_kind(store):
    out = {}
    for e in store.all():
        out[e.get("kind", "")] = out.get(e.get("kind", ""), 0) + 1
    return out


def per_actor(store):
    out = {}
    for e in store.all():
        out[e.get("actor", "")] = out.get(e.get("actor", ""), 0) + 1
    return out


def per_review(store):
    out = {}
    for e in store.all():
        out[e.get("review_id", "")] = out.get(e.get("review_id", ""), 0) + 1
    return out

"""view: helper views over ReactionStore."""
from __future__ import annotations


def per_reviewer_counts(store):
    out = {}
    for r in store.all():
        out[r.reviewer_id] = out.get(r.reviewer_id, 0) + 1
    return out


def per_emoji_counts(store):
    out = {}
    for r in store.all():
        out[r.emoji] = out.get(r.emoji, 0) + 1
    return out


def unique_targets(store):
    return sorted({r.target_id for r in store.all()})


def unique_reviewers(store):
    return sorted({r.reviewer_id for r in store.all()})

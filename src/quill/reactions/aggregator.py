"""aggregator: per-target reaction counts."""
from __future__ import annotations


def counts_by_emoji(store, target_id):
    out = {}
    for r in store.for_target(target_id):
        out[r.emoji] = out.get(r.emoji, 0) + 1
    return out


def total_for_target(store, target_id):
    return len(store.for_target(target_id))


def top_emojis(store, target_id, *, n=5):
    counts = counts_by_emoji(store, target_id)
    return sorted(counts.items(), key=lambda kv: -kv[1])[:n]


def emojis_by_reviewer(store, reviewer_id):
    return sorted({r.emoji for r in store.by_reviewer(reviewer_id)})

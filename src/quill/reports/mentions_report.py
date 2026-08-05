"""mentions_report: per-review mentions summary."""
from __future__ import annotations

from quill.mentions.parser import parse


def per_reviewer_mention_counts(store, review_id):
    out = {}
    for c in store.for_review(review_id):
        for m in parse(c.body):
            out[m] = out.get(m, 0) + 1
    return out


def top_mentioned(store, review_id, *, n=5):
    counts = per_reviewer_mention_counts(store, review_id)
    return sorted(counts.items(), key=lambda kv: -kv[1])[:n]


def render(store, review_id, *, width=60):
    counts = per_reviewer_mention_counts(store, review_id)
    out = ["=" * width]
    out.append(("MENTIONS " + review_id).center(width))
    out.append("=" * width)
    for mention, count in sorted(counts.items(), key=lambda kv: -kv[1]):
        out.append("  @" + mention.ljust(20) + str(count))
    return "\n".join(out)

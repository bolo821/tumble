"""report: per-review suggestion summary."""
from __future__ import annotations


def summary_for_review(store, review_id):
    items = store.for_review(review_id)
    pending = [s for s in items if s.is_pending()]
    applied = [s for s in items if s.is_applied()]
    discarded = [s for s in items if s.is_discarded()]
    return {
        "total": len(items),
        "pending": len(pending),
        "applied": len(applied),
        "discarded": len(discarded),
    }


def render(store, review_id, *, width=60):
    s = summary_for_review(store, review_id)
    out = ["=" * width]
    out.append(("SUGGESTIONS " + review_id).center(width))
    out.append("=" * width)
    for k, v in s.items():
        out.append(k.ljust(15) + str(v))
    return "\n".join(out)

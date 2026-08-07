"""health_report: overall system health snapshot."""
from __future__ import annotations


def health(*, review_store, heartbeat_store, now):
    from quill.heartbeat.registry import liveness_ratio
    return {
        "total_reviews": len(review_store),
        "open_reviews": sum(1 for r in review_store.all() if r.is_open()),
        "liveness_ratio": liveness_ratio(heartbeat_store, now=now),
    }


def render(*, review_store, heartbeat_store, now, width=60):
    h = health(review_store=review_store,
               heartbeat_store=heartbeat_store, now=now)
    out = ["=" * width]
    out.append("HEALTH".center(width))
    out.append("=" * width)
    for k, v in h.items():
        out.append(k.ljust(20) + str(v))
    return "\n".join(out)

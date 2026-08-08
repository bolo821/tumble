"""aggregate_view: cross-store aggregation view."""
from __future__ import annotations

from quill.history.aggregate import per_kind, per_actor


def summary(store):
    return {
        "total": len(store),
        "by_kind": per_kind(store),
        "by_actor": per_actor(store),
    }


def render(store, *, width=60):
    s = summary(store)
    out = ["=" * width]
    out.append("HISTORY SUMMARY".center(width))
    out.append("=" * width)
    out.append("total: " + str(s["total"]))
    out.append("by kind:")
    for kind, count in sorted(s["by_kind"].items()):
        out.append("  " + kind.ljust(20) + str(count))
    return "\n".join(out)

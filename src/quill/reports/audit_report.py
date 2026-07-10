"""audit_report: audit summary."""
from __future__ import annotations

from quill.audit.aggregator import total_by_actor, total_by_kind


def summary(log):
    return {
        "total": len(log),
        "by_actor": total_by_actor(log),
        "by_kind": total_by_kind(log),
    }


def render(log, *, width=60):
    out = ["=" * width]
    out.append("AUDIT REPORT".center(width))
    out.append("=" * width)
    s = summary(log)
    out.append("total: " + str(s["total"]))
    out.append("actors:")
    for actor, count in sorted(s["by_actor"].items()):
        out.append("  " + actor.ljust(20) + str(count))
    return "\n".join(out)

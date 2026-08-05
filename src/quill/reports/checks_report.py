"""checks_report: per-review checks status."""
from __future__ import annotations

from quill.checks.summary import suite_status
from quill.checks.aggregator import status_counts


def summary(store, review_id):
    return {
        "review_id": review_id,
        "suite_status": suite_status(store, review_id),
        "status_counts": status_counts(store, review_id),
    }


def render(store, review_id, *, width=60):
    s = summary(store, review_id)
    out = ["=" * width]
    out.append(("CHECKS " + review_id).center(width))
    out.append("=" * width)
    out.append("suite: " + s["suite_status"])
    for status, count in sorted(s["status_counts"].items()):
        out.append("  " + status.ljust(15) + str(count))
    return "\n".join(out)

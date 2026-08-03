"""formatter: format progress as a bar."""
from __future__ import annotations


def bar(tracker, review_id, *, width=20, filled="#", empty="."):
    percent = tracker.percent_done(review_id)
    filled_count = (percent * width) // 100
    return filled * filled_count + empty * (width - filled_count)


def summary(tracker, review_id):
    return {
        "done": tracker.gates_done(review_id),
        "total": tracker.gates_total(review_id),
        "percent": tracker.percent_done(review_id),
        "complete": tracker.is_complete(review_id),
    }


def render(tracker, review_id, *, width=60):
    out = ["=" * width]
    out.append(("PROGRESS " + review_id).center(width))
    out.append("=" * width)
    s = summary(tracker, review_id)
    out.append("  " + bar(tracker, review_id))
    out.append("  " + str(s["percent"]) + "%  (" + str(s["done"]) + "/" + str(s["total"]) + ")")
    return "\n".join(out)

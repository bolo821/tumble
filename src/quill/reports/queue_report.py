"""queue_report: per-MergeQueue summary."""
from __future__ import annotations


def summary(queue):
    return {
        "depth": queue.depth(),
        "peek": queue.peek(),
        "ids": queue.ids(),
    }


def render(queue, *, width=60):
    out = ["=" * width]
    out.append("MERGE QUEUE".center(width))
    out.append("=" * width)
    out.append("depth: " + str(queue.depth()))
    for i, rid in enumerate(queue.ids()):
        out.append("  " + str(i + 1).rjust(3) + ". " + rid)
    return "\n".join(out)

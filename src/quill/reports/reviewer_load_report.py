"""reviewer_load_report: per-reviewer load summary."""
from __future__ import annotations

from quill.reviewers.load import per_reviewer_load


def summary(assignment_store, reviewers):
    return per_reviewer_load(assignment_store, reviewers)


def render(assignment_store, reviewers, *, width=60):
    loads = summary(assignment_store, reviewers)
    out = ["=" * width]
    out.append("REVIEWER LOAD".center(width))
    out.append("=" * width)
    for reviewer, load in sorted(loads.items(), key=lambda kv: -kv[1]):
        out.append("  " + reviewer.ljust(20) + str(load))
    return "\n".join(out)

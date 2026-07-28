"""reviewer_report: per-reviewer stats."""
from __future__ import annotations


def per_reviewer_counts(assignment_store):
    out = {}
    for rid, ids in assignment_store._assignments.items():
        for reviewer in ids:
            out[reviewer] = out.get(reviewer, 0) + 1
    return out


def per_reviewer_open_counts(assignment_store, review_store):
    from quill.reviewers.load import open_load
    out = {}
    for rid, ids in assignment_store._assignments.items():
        for reviewer in ids:
            out.setdefault(reviewer, 0)
    # tally in a second pass
    for reviewer in out:
        out[reviewer] = open_load(assignment_store,
                                   reviewer_id=reviewer,
                                   review_store=review_store)
    return out


def render(assignment_store, *, width=60):
    out = ["=" * width]
    out.append("REVIEWER REPORT".center(width))
    out.append("=" * width)
    for reviewer, count in sorted(per_reviewer_counts(assignment_store).items()):
        out.append(reviewer.ljust(20) + str(count))
    return "\n".join(out)

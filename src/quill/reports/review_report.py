"""review_report: per-review summary."""
from __future__ import annotations


def summary(review):
    return {
        "id": review.id,
        "title": review.title,
        "author": review.author,
        "status": review.status,
        "base_branch": review.base_branch,
        "head_branch": review.head_branch,
        "additions": review.additions,
        "deletions": review.deletions,
    }


def render(review, *, width=60):
    s = summary(review)
    out = ["=" * width]
    out.append(("REVIEW " + review.id).center(width))
    out.append("=" * width)
    for k, v in s.items():
        out.append(k.ljust(20) + str(v))
    return "\n".join(out)


def total_delta(review):
    return review.additions + review.deletions

"""needs_approval: reviews that still need approvals."""
from __future__ import annotations

from quill.approvals.policy import is_satisfied, pending_count


def reviews_needing_more(review_store, approval_log, *, required_count):
    out = []
    for r in review_store.all():
        if r.is_terminal():
            continue
        if is_satisfied(log=approval_log, review_id=r.id,
                        required_count=required_count):
            continue
        out.append(r)
    return out


def per_review_pending_count(review_store, approval_log, *, required_count):
    out = {}
    for r in review_store.all():
        if r.is_terminal():
            continue
        out[r.id] = pending_count(log=approval_log, review_id=r.id,
                                   required_count=required_count)
    return out

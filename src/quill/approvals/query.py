"""query: convenience queries over ApprovalLog."""
from __future__ import annotations

from quill.approvals.approval import APPROVE, REQUEST_CHANGES


def approvers(log, *, review_id):
    return sorted({a.reviewer for a in log.for_review(review_id)
                   if a.is_approve()})


def change_requesters(log, *, review_id):
    return sorted({a.reviewer for a in log.for_review(review_id)
                   if a.is_request_changes()})


def total_by_decision(log):
    out = {}
    for a in log.all():
        out[a.decision] = out.get(a.decision, 0) + 1
    return out


def per_reviewer_approvals(log):
    out = {}
    for a in log.all():
        if a.is_approve():
            out[a.reviewer] = out.get(a.reviewer, 0) + 1
    return out


def most_active_reviewers(log, *, n=5):
    counts = per_reviewer_approvals(log)
    return sorted(counts.items(), key=lambda kv: -kv[1])[:n]

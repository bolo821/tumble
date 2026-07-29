"""dismiss: dismiss stale approvals."""
from __future__ import annotations

from quill.approvals.approval import Approval, COMMENT_ONLY


def dismiss(log, *, review_id, at, actor):
    """Convert every approval for review_id into a comment-only marker."""
    dismissed = []
    latest_by_reviewer = {}
    for a in log.for_review(review_id):
        latest_by_reviewer[a.reviewer] = a
    for reviewer, a in latest_by_reviewer.items():
        if a.is_approve():
            log.record(Approval(
                review_id=review_id, reviewer=reviewer,
                decision=COMMENT_ONLY, at=at,
                body="dismissed by " + actor,
            ))
            dismissed.append(reviewer)
    return dismissed


def has_been_dismissed(log, *, review_id, reviewer):
    for a in log.for_review(review_id):
        if a.reviewer == reviewer and a.body.startswith("dismissed"):
            return True
    return False

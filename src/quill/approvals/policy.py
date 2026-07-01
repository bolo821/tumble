"""policy: evaluate N-of-M against collected approvals."""
from __future__ import annotations

from quill.approvals.approval import APPROVE, REQUEST_CHANGES


def _latest_by_reviewer(approvals, review_id):
    """Only the most recent decision per reviewer counts."""
    seen = {}
    for a in approvals:
        if a.review_id != review_id:
            continue
        seen[a.reviewer] = a
    return seen


def is_satisfied(*, log, review_id, required_count, required_reviewers=()):
    latest = _latest_by_reviewer(log.all(), review_id)
    if any(a.decision == REQUEST_CHANGES for a in latest.values()):
        return False
    approvers = {r for r, a in latest.items() if a.decision == APPROVE}
    if required_reviewers and not set(required_reviewers) <= approvers:
        return False
    return len(approvers) >= required_count


def has_blocking_change_request(*, log, review_id):
    latest = _latest_by_reviewer(log.all(), review_id)
    return any(a.decision == REQUEST_CHANGES for a in latest.values())


def approver_set(*, log, review_id):
    latest = _latest_by_reviewer(log.all(), review_id)
    return {r for r, a in latest.items() if a.decision == APPROVE}


def pending_count(*, log, review_id, required_count):
    approvers = approver_set(log=log, review_id=review_id)
    return max(0, required_count - len(approvers))

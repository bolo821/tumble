"""approvals_snapshot: snapshot ApprovalLog state."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class ApprovalsSnapshot:
    at: int
    counts_by_decision: dict
    counts_by_reviewer: dict


def from_log(log, *, at):
    from quill.approvals.query import total_by_decision, per_reviewer_approvals
    return ApprovalsSnapshot(
        at=at,
        counts_by_decision=dict(total_by_decision(log)),
        counts_by_reviewer=dict(per_reviewer_approvals(log)),
    )


def total(snapshot):
    return sum(snapshot.counts_by_decision.values())


def top_reviewer(snapshot):
    if not snapshot.counts_by_reviewer:
        return None
    return max(snapshot.counts_by_reviewer.items(), key=lambda kv: kv[1])[0]

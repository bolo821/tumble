"""ApprovalLog: append-only decisions."""
from __future__ import annotations


class ApprovalLog:
    def __init__(self):
        self._approvals = []

    def record(self, approval):
        self._approvals.append(approval)
        return self

    def all(self):
        return list(self._approvals)

    def for_review(self, review_id):
        return [a for a in self._approvals if a.review_id == review_id]

    def by_reviewer(self, reviewer):
        return [a for a in self._approvals if a.reviewer == reviewer]

    def latest_for(self, *, review_id, reviewer):
        matches = [a for a in self._approvals
                   if a.review_id == review_id and a.reviewer == reviewer]
        return matches[-1] if matches else None

    def __len__(self):
        return len(self._approvals)

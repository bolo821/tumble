"""Approval: a reviewer's decision on a review."""
from __future__ import annotations

from dataclasses import dataclass


APPROVE = "approve"
REQUEST_CHANGES = "request_changes"
COMMENT_ONLY = "comment"


@dataclass(frozen=True)
class Approval:
    review_id: str
    reviewer: str
    decision: str
    at: int
    body: str = ""

    def is_approve(self):
        return self.decision == APPROVE

    def is_request_changes(self):
        return self.decision == REQUEST_CHANGES

    def is_comment_only(self):
        return self.decision == COMMENT_ONLY

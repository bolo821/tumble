"""Suggestion: a proposed diff attached to a Comment."""
from __future__ import annotations

from dataclasses import dataclass


PENDING = "pending"
APPLIED = "applied"
DISCARDED = "discarded"


@dataclass
class Suggestion:
    id: str
    comment_id: str
    review_id: str
    file_path: str
    original_lines: tuple
    proposed_lines: tuple
    status: str = PENDING
    applied_at: int = 0
    applied_by: str = ""

    def is_pending(self):
        return self.status == PENDING

    def is_applied(self):
        return self.status == APPLIED

    def is_discarded(self):
        return self.status == DISCARDED

    def line_delta(self):
        return len(self.proposed_lines) - len(self.original_lines)

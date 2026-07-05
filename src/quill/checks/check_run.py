"""CheckRun: an external check reporting a status."""
from __future__ import annotations

from dataclasses import dataclass


PENDING = "pending"
SUCCESS = "success"
FAILURE = "failure"
NEUTRAL = "neutral"
CANCELLED = "cancelled"


@dataclass(frozen=True)
class CheckRun:
    id: str
    review_id: str
    name: str
    status: str
    started_at: int = 0
    finished_at: int = 0
    conclusion_url: str = ""

    def is_terminal(self):
        return self.status in (SUCCESS, FAILURE, NEUTRAL, CANCELLED)

    def is_success(self):
        return self.status == SUCCESS

    def is_failure(self):
        return self.status == FAILURE

    def duration(self):
        if self.finished_at == 0:
            return 0
        return max(0, self.finished_at - self.started_at)

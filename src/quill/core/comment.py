"""Comment: a comment on a review."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass
class Comment:
    id: str
    review_id: str
    thread_id: str
    author: str
    body: str
    file_path: str = ""
    line: int = 0
    created_at: int = 0
    resolved: bool = False
    is_suggestion: bool = False

    def is_inline(self):
        return bool(self.file_path) and self.line > 0

    def short_body(self):
        return self.body[:80]

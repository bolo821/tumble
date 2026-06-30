"""Thread: a group of related Comments."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass
class Thread:
    id: str
    review_id: str
    file_path: str = ""
    line: int = 0
    resolved: bool = False
    created_at: int = 0

    def is_inline(self):
        return bool(self.file_path) and self.line > 0

    def is_general(self):
        return not self.is_inline()

"""FileChange: per-file diff summary."""
from __future__ import annotations

from dataclasses import dataclass


ADDED = "added"
MODIFIED = "modified"
DELETED = "deleted"
RENAMED = "renamed"


@dataclass(frozen=True)
class FileChange:
    review_id: str
    path: str
    kind: str  # added | modified | deleted | renamed
    additions: int = 0
    deletions: int = 0
    old_path: str = ""

    def total_lines(self):
        return self.additions + self.deletions

    def is_binary_delete(self):
        return self.kind == DELETED and self.additions == 0

    def is_rename(self):
        return self.kind == RENAMED

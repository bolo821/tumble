"""history: track comment edits."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class EditRecord:
    comment_id: str
    at: int
    editor: str
    prev_body: str


class EditHistory:
    def __init__(self):
        self._records = []

    def record(self, edit):
        self._records.append(edit)
        return self

    def for_comment(self, comment_id):
        return [r for r in self._records if r.comment_id == comment_id]

    def edit_count(self, comment_id):
        return len(self.for_comment(comment_id))

    def __len__(self):
        return len(self._records)

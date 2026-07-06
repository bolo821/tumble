"""DraftComment: an unpublished comment."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass
class DraftComment:
    id: str
    review_id: str
    author: str
    body: str
    file_path: str = ""
    line: int = 0
    created_at: int = 0

    def is_inline(self):
        return bool(self.file_path) and self.line > 0


class DraftStore:
    def __init__(self):
        self._by_id = {}

    def stash(self, draft):
        self._by_id[draft.id] = draft
        return self

    def get(self, draft_id):
        return self._by_id.get(draft_id)

    def for_author(self, author):
        return [d for d in self._by_id.values() if d.author == author]

    def for_review(self, review_id):
        return [d for d in self._by_id.values() if d.review_id == review_id]

    def drop(self, draft_id):
        self._by_id.pop(draft_id, None)
        return self

    def __len__(self):
        return len(self._by_id)

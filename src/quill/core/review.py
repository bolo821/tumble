"""Review: the central pull-request record."""
from __future__ import annotations

from dataclasses import dataclass

from quill.core.status import ReviewStatus


@dataclass
class Review:
    id: str
    title: str
    author: str
    base_branch: str = "main"
    head_branch: str = ""
    status: str = ReviewStatus.DRAFT
    created_at: int = 0
    updated_at: int = 0
    labels: tuple = ()
    changed_files: tuple = ()
    additions: int = 0
    deletions: int = 0

    def label_set(self):
        return set(self.labels)

    def has_label(self, label):
        return label in self.label_set()

    def is_open(self):
        return self.status == ReviewStatus.OPEN

    def is_approved(self):
        return self.status == ReviewStatus.APPROVED

    def is_merged(self):
        return self.status == ReviewStatus.MERGED

    def is_terminal(self):
        return ReviewStatus.is_terminal(self.status)


def make_review(*, id, title, author, **kwargs):
    return Review(id=id, title=title, author=author, **kwargs)

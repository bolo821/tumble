"""ReviewStatus enum and helpers."""
from __future__ import annotations


class ReviewStatus:
    DRAFT = "draft"
    OPEN = "open"
    APPROVED = "approved"
    MERGED = "merged"
    CLOSED = "closed"

    TERMINAL = frozenset({MERGED, CLOSED})
    ACTIVE = frozenset({DRAFT, OPEN, APPROVED})
    MERGEABLE_FROM = frozenset({APPROVED})

    @classmethod
    def is_terminal(cls, status):
        return status in cls.TERMINAL

    @classmethod
    def is_active(cls, status):
        return status in cls.ACTIVE

    @classmethod
    def is_mergeable(cls, status):
        return status in cls.MERGEABLE_FROM

"""Top-level errors for quill."""
from __future__ import annotations


class QuillError(Exception):
    pass


class UnknownReview(QuillError):
    pass


class UnknownComment(QuillError):
    pass


class UnknownThread(QuillError):
    pass


class InvalidTransition(QuillError):
    pass


class DuplicateReview(QuillError):
    pass


class MergeBlocked(QuillError):
    pass


class ReviewClosed(QuillError):
    pass


class ApprovalMissing(QuillError):
    pass

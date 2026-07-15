"""diff: compare two checkpoints."""
from __future__ import annotations


def review_delta(before, after):
    if before is None or after is None:
        return 0
    return after.review_count - before.review_count


def comment_delta(before, after):
    if before is None or after is None:
        return 0
    return after.comment_count - before.comment_count


def approval_delta(before, after):
    if before is None or after is None:
        return 0
    return after.approval_count - before.approval_count


def time_delta(before, after):
    if before is None or after is None:
        return 0
    return after.at - before.at

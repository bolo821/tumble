"""time_filter: time-based predicates."""
from __future__ import annotations


def created_after(threshold):
    def pred(review):
        return review.created_at >= threshold
    return pred


def created_before(threshold):
    def pred(review):
        return review.created_at < threshold
    return pred


def updated_after(threshold):
    def pred(review):
        return review.updated_at >= threshold
    return pred


def in_window(start, end):
    def pred(review):
        return start <= review.created_at < end
    return pred


def older_than(seconds, *, now):
    def pred(review):
        return (now - review.created_at) > seconds
    return pred

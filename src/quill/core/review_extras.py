"""review_extras: helpers around Review objects."""
from __future__ import annotations


def total_delta(review):
    return review.additions + review.deletions


def by_status(reviews, status):
    return [r for r in reviews if r.status == status]


def active_reviews(reviews):
    return [r for r in reviews if not r.is_terminal()]


def terminal_reviews(reviews):
    return [r for r in reviews if r.is_terminal()]


def slowest(reviews, *, n=5):
    open_ones = [r for r in reviews if r.created_at]
    return sorted(open_ones, key=lambda r: r.created_at)[:n]


def average_deltas(reviews):
    if not reviews:
        return 0
    return sum(total_delta(r) for r in reviews) // len(reviews)

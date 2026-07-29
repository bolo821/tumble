"""estimator: estimate wait time for a queued review."""
from __future__ import annotations


def estimated_wait_seconds(queue, *, review_id, per_review_seconds):
    pos = queue.position(review_id)
    if pos < 0:
        return None
    return pos * per_review_seconds


def estimated_completion_time(queue, *, review_id, per_review_seconds, now):
    wait = estimated_wait_seconds(queue, review_id=review_id,
                                  per_review_seconds=per_review_seconds)
    if wait is None:
        return None
    return now + wait + per_review_seconds


def total_estimated_time(queue, *, per_review_seconds):
    return queue.depth() * per_review_seconds

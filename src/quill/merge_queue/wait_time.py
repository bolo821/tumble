"""wait_time: how long a review has been queued."""
from __future__ import annotations


def enqueue_time(*, observer, review_id):
    for e in observer.for_review(review_id):
        if e["kind"] == "enqueue":
            return e["at"]
    return None


def dequeue_time(*, observer, review_id):
    for e in observer.for_review(review_id):
        if e["kind"] == "dequeue":
            return e["at"]
    return None


def wait_seconds(*, observer, review_id, now=0):
    start = enqueue_time(observer=observer, review_id=review_id)
    if start is None:
        return 0
    end = dequeue_time(observer=observer, review_id=review_id)
    if end is None:
        end = now
    return max(0, end - start)


def average_wait(observer, *, now=0):
    review_ids = sorted({e["review_id"] for e in observer.events()})
    if not review_ids:
        return 0
    total = 0
    for rid in review_ids:
        total += wait_seconds(observer=observer, review_id=rid, now=now)
    return total // len(review_ids)

"""aggregator: per-review CI-check aggregates."""
from __future__ import annotations

from quill.checks.check_run import SUCCESS, FAILURE, PENDING


def status_counts(store, review_id):
    out = {}
    for r in store.for_review(review_id):
        out[r.status] = out.get(r.status, 0) + 1
    return out


def success_count(store, review_id):
    return sum(1 for r in store.for_review(review_id) if r.status == SUCCESS)


def failure_count(store, review_id):
    return sum(1 for r in store.for_review(review_id) if r.status == FAILURE)


def is_all_green(store, review_id):
    for r in store.for_review(review_id):
        if r.status != SUCCESS:
            return False
    return True


def total_duration(store, review_id):
    return sum(r.duration() for r in store.for_review(review_id))

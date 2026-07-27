"""load: per-reviewer load metrics."""
from __future__ import annotations


def open_load(assignment_store, *, reviewer_id, review_store):
    review_ids = assignment_store.for_reviewer(reviewer_id)
    open_count = 0
    for rid in review_ids:
        r = review_store.get_or_none(rid)
        if r is not None and r.is_open():
            open_count += 1
    return open_count


def total_load(assignment_store, *, reviewer_id):
    return len(assignment_store.for_reviewer(reviewer_id))


def per_reviewer_load(assignment_store, reviewers):
    out = {}
    for r in reviewers:
        out[r.id] = total_load(assignment_store, reviewer_id=r.id)
    return out


def least_loaded(assignment_store, candidates):
    if not candidates:
        return None
    return min(candidates, key=lambda r: total_load(assignment_store, reviewer_id=r.id))

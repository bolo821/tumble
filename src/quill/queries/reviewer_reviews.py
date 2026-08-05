"""reviewer_reviews: queries per reviewer."""
from __future__ import annotations


def all_for_reviewer(assignment_store, review_store, *, reviewer_id):
    review_ids = assignment_store.for_reviewer(reviewer_id)
    return [review_store.get_or_none(rid) for rid in review_ids
            if review_store.get_or_none(rid) is not None]


def open_for_reviewer(assignment_store, review_store, *, reviewer_id):
    return [r for r in all_for_reviewer(assignment_store, review_store,
                                        reviewer_id=reviewer_id)
            if r.is_open()]


def approved_for_reviewer(assignment_store, review_store, *, reviewer_id):
    return [r for r in all_for_reviewer(assignment_store, review_store,
                                        reviewer_id=reviewer_id)
            if r.is_approved()]


def waiting_for_reviewer(assignment_store, review_store, *, reviewer_id, approval_log):
    open_reviews = open_for_reviewer(assignment_store, review_store,
                                     reviewer_id=reviewer_id)
    out = []
    for r in open_reviews:
        given = any(a.reviewer == reviewer_id for a in approval_log.for_review(r.id))
        if not given:
            out.append(r)
    return out

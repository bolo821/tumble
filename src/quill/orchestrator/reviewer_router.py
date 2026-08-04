"""reviewer_router: route reviewers to a review."""
from __future__ import annotations

from quill.reviewers.load import least_loaded


class ReviewerRouter:
    def __init__(self, *, assignment_store, rotation):
        self._assignments = assignment_store
        self._rotation = rotation

    def route_least_loaded(self, *, review_id, candidates):
        if not candidates:
            return None
        chosen = least_loaded(self._assignments, list(candidates))
        if chosen is None:
            return None
        self._assignments.assign(review_id=review_id, reviewer_id=chosen.id)
        return chosen

    def route_round_robin(self, *, review_id, pool_key, candidate_ids):
        chosen_id = self._rotation.next(pool_key=pool_key,
                                        candidates=candidate_ids)
        if chosen_id is None:
            return None
        self._assignments.assign(review_id=review_id, reviewer_id=chosen_id)
        return chosen_id

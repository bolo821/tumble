"""AssignmentStore: per-review reviewer assignments."""
from __future__ import annotations


class AssignmentStore:
    def __init__(self):
        self._assignments = {}  # review_id -> set(reviewer_id)

    def assign(self, *, review_id, reviewer_id):
        self._assignments.setdefault(review_id, set()).add(reviewer_id)
        return self

    def unassign(self, *, review_id, reviewer_id):
        if review_id in self._assignments:
            self._assignments[review_id].discard(reviewer_id)
        return self

    def for_review(self, review_id):
        return sorted(self._assignments.get(review_id, set()))

    def for_reviewer(self, reviewer_id):
        return sorted(r for r, ids in self._assignments.items() if reviewer_id in ids)

    def is_assigned(self, *, review_id, reviewer_id):
        return reviewer_id in self._assignments.get(review_id, set())

    def total_assignments(self):
        return sum(len(v) for v in self._assignments.values())

    def review_count(self):
        return len(self._assignments)

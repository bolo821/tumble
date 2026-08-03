"""ProgressTracker: per-review completion."""
from __future__ import annotations


class ProgressTracker:
    def __init__(self):
        self._total = {}
        self._done = {}

    def declare_total(self, *, review_id, total_gates):
        self._total[review_id] = total_gates
        self._done.setdefault(review_id, set())
        return self

    def mark_gate_done(self, *, review_id, gate_name):
        self._done.setdefault(review_id, set()).add(gate_name)
        return self

    def gates_done(self, review_id):
        return len(self._done.get(review_id, set()))

    def gates_total(self, review_id):
        return self._total.get(review_id, 0)

    def percent_done(self, review_id):
        total = self.gates_total(review_id)
        if total == 0:
            return 0
        return int(100 * self.gates_done(review_id) / total)

    def is_complete(self, review_id):
        return self.gates_done(review_id) == self.gates_total(review_id) > 0

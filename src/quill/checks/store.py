"""CheckRunStore: per-review check-run storage."""
from __future__ import annotations


class CheckRunStore:
    def __init__(self):
        self._by_id = {}

    def add(self, run):
        self._by_id[run.id] = run
        return self

    def get(self, run_id):
        return self._by_id.get(run_id)

    def for_review(self, review_id):
        return [r for r in self._by_id.values() if r.review_id == review_id]

    def by_name(self, review_id, *, name):
        return [r for r in self.for_review(review_id) if r.name == name]

    def latest_for(self, review_id, *, name):
        runs = self.by_name(review_id, name=name)
        return runs[-1] if runs else None

    def __len__(self):
        return len(self._by_id)

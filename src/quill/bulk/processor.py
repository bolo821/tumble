"""BulkProcessor: execute BulkOps."""
from __future__ import annotations


class BulkProcessor:
    def __init__(self):
        self._completed = []
        self._failed = []

    def apply(self, *, op, store, executor):
        for rid in op.target_review_ids:
            review = store.get_or_none(rid)
            if review is None:
                self._failed.append({"review_id": rid, "reason": "missing"})
                continue
            try:
                executor(op, review)
                self._completed.append(rid)
            except Exception as e:
                self._failed.append({"review_id": rid, "reason": str(e)})
        return self

    def completed_count(self):
        return len(self._completed)

    def failed_count(self):
        return len(self._failed)

    def failures(self):
        return list(self._failed)

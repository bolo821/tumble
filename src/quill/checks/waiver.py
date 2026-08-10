"""waiver: per-check waiver bookkeeping."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class Waiver:
    review_id: str
    check_name: str
    actor: str
    reason: str
    at: int


class WaiverStore:
    def __init__(self):
        self._waivers = []

    def grant(self, waiver):
        self._waivers.append(waiver)
        return self

    def all(self):
        return list(self._waivers)

    def for_review(self, review_id):
        return [w for w in self._waivers if w.review_id == review_id]

    def is_waived(self, *, review_id, check_name):
        for w in self._waivers:
            if w.review_id == review_id and w.check_name == check_name:
                return True
        return False

    def revoke(self, *, review_id, check_name):
        self._waivers = [
            w for w in self._waivers
            if not (w.review_id == review_id and w.check_name == check_name)
        ]
        return self

    def __len__(self):
        return len(self._waivers)

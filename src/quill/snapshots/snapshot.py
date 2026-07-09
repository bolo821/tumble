"""Snapshot: a frozen review-state view."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class Snapshot:
    at: int
    review_payload: tuple = ()  # tuple of {"id","status","author"}

    def review_count(self):
        return len(self.review_payload)

    def status_counts(self):
        out = {}
        for r in self.review_payload:
            out[r["status"]] = out.get(r["status"], 0) + 1
        return out

    def ids(self):
        return [r["id"] for r in self.review_payload]

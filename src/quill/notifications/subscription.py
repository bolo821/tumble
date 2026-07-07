"""Subscription: per-reviewer subscription."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class Subscription:
    reviewer_id: str
    review_id: str = ""
    label: str = ""
    event_kind: str = "open"


class SubscriptionStore:
    def __init__(self):
        self._subs = []

    def subscribe(self, sub):
        self._subs.append(sub)
        return self

    def unsubscribe(self, *, reviewer_id, review_id="", label="", event_kind="open"):
        self._subs = [
            s for s in self._subs
            if not (s.reviewer_id == reviewer_id and s.review_id == review_id
                    and s.label == label and s.event_kind == event_kind)
        ]
        return self

    def for_reviewer(self, reviewer_id):
        return [s for s in self._subs if s.reviewer_id == reviewer_id]

    def for_review(self, review_id):
        return [s for s in self._subs if s.review_id == review_id]

    def __len__(self):
        return len(self._subs)

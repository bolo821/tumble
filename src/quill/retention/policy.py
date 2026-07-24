"""RetentionPolicy: how long to keep."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class RetentionPolicy:
    label: str
    keep_seconds: int

    def must_expire_by(self, *, created_at):
        if self.keep_seconds <= 0:
            return 0
        return created_at + self.keep_seconds


class PolicyRegistry:
    def __init__(self):
        self._by_label = {}

    def register(self, policy):
        self._by_label[policy.label] = policy
        return self

    def get(self, label):
        return self._by_label.get(label)

    def labels(self):
        return sorted(self._by_label)

    def __len__(self):
        return len(self._by_label)

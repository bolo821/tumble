"""BulkOp: a request to apply an op."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class BulkOp:
    kind: str
    target_review_ids: tuple
    parameters: tuple = ()

    def parameter_dict(self):
        return dict(self.parameters)

    def total_targets(self):
        return len(self.target_review_ids)

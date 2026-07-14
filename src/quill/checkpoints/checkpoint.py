"""Checkpoint: a frozen recovery point."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class Checkpoint:
    sequence: int
    at: int
    review_count: int
    comment_count: int = 0
    approval_count: int = 0

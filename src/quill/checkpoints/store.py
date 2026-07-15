"""CheckpointStore: time-ordered checkpoint history."""
from __future__ import annotations


class CheckpointStore:
    def __init__(self):
        self._checkpoints = []

    def record(self, checkpoint):
        self._checkpoints.append(checkpoint)
        return self

    def latest(self):
        return self._checkpoints[-1] if self._checkpoints else None

    def all(self):
        return list(self._checkpoints)

    def __len__(self):
        return len(self._checkpoints)

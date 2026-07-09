"""SnapshotStore: time-ordered snapshot history."""
from __future__ import annotations


class SnapshotStore:
    def __init__(self):
        self._snapshots = []

    def record(self, snapshot):
        self._snapshots.append(snapshot)
        return self

    def latest(self):
        return self._snapshots[-1] if self._snapshots else None

    def all(self):
        return list(self._snapshots)

    def in_window(self, *, start, end):
        return [s for s in self._snapshots if start <= s.at < end]

    def __len__(self):
        return len(self._snapshots)

"""SnapshotIndex: per-snapshot review-id index."""
from __future__ import annotations


class SnapshotIndex:
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

    def review_history(self, review_id):
        out = []
        for s in self._snapshots:
            for r in s.review_payload:
                if r["id"] == review_id:
                    out.append((s.at, r))
                    break
        return out

    def __len__(self):
        return len(self._snapshots)

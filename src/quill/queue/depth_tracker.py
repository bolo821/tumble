"""DepthTracker: track queue depth over time."""
from __future__ import annotations


class DepthTracker:
    def __init__(self):
        self._samples = []

    def record(self, *, at, depth):
        self._samples.append((at, depth))
        return self

    def peak(self):
        if not self._samples:
            return None
        return max(self._samples, key=lambda kv: kv[1])

    def average(self):
        if not self._samples:
            return 0
        return sum(d for _, d in self._samples) // len(self._samples)

    def at(self, *, t):
        relevant = [s for s in self._samples if s[0] <= t]
        return relevant[-1][1] if relevant else 0

    def __len__(self):
        return len(self._samples)

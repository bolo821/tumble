"""rotation: pick a reviewer round-robin from an available pool."""
from __future__ import annotations


class RotationState:
    def __init__(self):
        self._last = {}  # pool_key -> last-index

    def next(self, *, pool_key, candidates):
        if not candidates:
            return None
        candidates = sorted(candidates)
        last_idx = self._last.get(pool_key, -1)
        idx = (last_idx + 1) % len(candidates)
        self._last[pool_key] = idx
        return candidates[idx]

    def last_picked(self, pool_key):
        idx = self._last.get(pool_key)
        return idx if idx is not None else -1

    def reset(self, pool_key):
        self._last.pop(pool_key, None)
        return self

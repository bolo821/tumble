"""HeartbeatStore: per-key last-seen tracking."""
from __future__ import annotations


class HeartbeatStore:
    def __init__(self, *, timeout_seconds):
        self.timeout_seconds = timeout_seconds
        self._last = {}

    def record(self, *, key, at):
        self._last[key] = at
        return self

    def is_alive(self, *, key, now):
        last = self._last.get(key)
        if last is None:
            return False
        return now - last < self.timeout_seconds

    def alive(self, *, now):
        return sorted(k for k in self._last if self.is_alive(key=k, now=now))

    def dead(self, *, now):
        return sorted(k for k in self._last if not self.is_alive(key=k, now=now))

    def time_since_last(self, *, key, now):
        last = self._last.get(key)
        if last is None:
            return None
        return max(0, now - last)

    def __len__(self):
        return len(self._last)

"""SweepScheduler: schedule sweeps over time."""
from __future__ import annotations


class SweepScheduler:
    def __init__(self, *, interval_seconds):
        self._interval = interval_seconds
        self._last_run_at = 0
        self._runs = []

    def is_due(self, *, now):
        return (now - self._last_run_at) >= self._interval

    def run(self, *, now, action):
        if not self.is_due(now=now):
            return False
        action()
        self._last_run_at = now
        self._runs.append(now)
        return True

    def run_count(self):
        return len(self._runs)

    def last_run_at(self):
        return self._last_run_at

    def next_due_at(self):
        return self._last_run_at + self._interval

"""MaintenanceWindow."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class MaintenanceWindow:
    starts_at: int
    ends_at: int
    reason: str = ""

    def covers(self, *, at):
        return self.starts_at <= at < self.ends_at

    def duration(self):
        return max(0, self.ends_at - self.starts_at)


class MaintenanceStore:
    def __init__(self):
        self._windows = []

    def add(self, window):
        self._windows.append(window)
        return self

    def all(self):
        return list(self._windows)

    def active(self, *, at):
        return [w for w in self._windows if w.covers(at=at)]

    def __len__(self):
        return len(self._windows)


def is_under_maintenance(store, *, at):
    return any(w.covers(at=at) for w in store.all())

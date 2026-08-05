"""head_tracker: track head-branch pointers over time."""
from __future__ import annotations


class HeadTracker:
    def __init__(self):
        self._history = {}

    def record(self, *, branch, sha, at):
        self._history.setdefault(branch, []).append((at, sha))
        return self

    def current(self, branch):
        history = self._history.get(branch, [])
        return history[-1][1] if history else None

    def history(self, branch):
        return list(self._history.get(branch, []))

    def branches(self):
        return sorted(self._history)

    def sha_at(self, *, branch, t):
        history = self._history.get(branch, [])
        relevant = [(at, sha) for at, sha in history if at <= t]
        return relevant[-1][1] if relevant else None

"""fair_dispatcher: fair per-author dispatch."""
from __future__ import annotations


class FairDispatcher:
    def __init__(self):
        self._last_pick = {}

    def next_author(self, candidates):
        if not candidates:
            return None
        return min(candidates, key=lambda a: self._last_pick.get(a, 0))

    def record_pick(self, *, author, at):
        self._last_pick[author] = at
        return self

    def wait_time(self, *, author, now):
        last = self._last_pick.get(author, 0)
        if last == 0:
            return 0
        return max(0, now - last)

    def authors(self):
        return sorted(self._last_pick)

"""WaitQueue: per-lock FIFO of waiters."""
from __future__ import annotations


class WaitQueue:
    def __init__(self):
        self._queues = {}

    def wait(self, *, name, waiter):
        self._queues.setdefault(name, []).append(waiter)
        return self

    def next_in_line(self, name):
        q = self._queues.get(name)
        return q[0] if q else None

    def pop_next(self, name):
        q = self._queues.get(name)
        if not q:
            return None
        return q.pop(0)

    def depth(self, name):
        return len(self._queues.get(name, []))

    def all_waiters(self, name):
        return list(self._queues.get(name, []))

"""WorkQueue: FIFO of work items."""
from __future__ import annotations


class WorkQueue:
    def __init__(self):
        self._queue = []

    def enqueue(self, item):
        self._queue.append(item)
        return self

    def dequeue(self):
        if not self._queue:
            return None
        return self._queue.pop(0)

    def peek(self):
        return self._queue[0] if self._queue else None

    def drain(self, *, max_items=0):
        if max_items <= 0:
            out = list(self._queue)
            self._queue.clear()
            return out
        out = self._queue[:max_items]
        self._queue = self._queue[max_items:]
        return out

    def depth(self):
        return len(self._queue)

    def is_empty(self):
        return not self._queue

    def __len__(self):
        return len(self._queue)

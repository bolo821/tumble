"""WorkDispatcher: dispatch queued items."""
from __future__ import annotations


class WorkDispatcher:
    def __init__(self, *, queue, handler):
        self._queue = queue
        self._handler = handler
        self._processed = []

    def dispatch_one(self):
        item = self._queue.dequeue()
        if item is None:
            return None
        self._handler(item)
        self._processed.append(item)
        return item

    def dispatch_all(self):
        out = []
        while True:
            item = self.dispatch_one()
            if item is None:
                break
            out.append(item)
        return out

    def processed_count(self):
        return len(self._processed)

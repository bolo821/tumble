"""PriorityWorkQueue: lower priority = earlier."""
from __future__ import annotations


class PriorityWorkQueue:
    def __init__(self):
        self._entries = []
        self._seq = 0

    def enqueue(self, item, *, priority=0):
        self._entries.append((priority, self._seq, item))
        self._seq += 1
        self._entries.sort(key=lambda kv: (kv[0], kv[1]))
        return self

    def dequeue(self):
        if not self._entries:
            return None
        return self._entries.pop(0)[2]

    def peek(self):
        if not self._entries:
            return None
        return self._entries[0][2]

    def depth(self):
        return len(self._entries)

    def items_at(self, priority):
        return [r for p, _, r in self._entries if p == priority]

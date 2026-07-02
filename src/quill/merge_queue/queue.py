"""MergeQueue: FIFO of reviews waiting to merge."""
from __future__ import annotations


class MergeQueue:
    def __init__(self):
        self._entries = []  # list of review_id

    def enqueue(self, review_id):
        if review_id in self._entries:
            return False
        self._entries.append(review_id)
        return True

    def dequeue(self):
        if not self._entries:
            return None
        return self._entries.pop(0)

    def peek(self):
        return self._entries[0] if self._entries else None

    def remove(self, review_id):
        try:
            self._entries.remove(review_id)
            return True
        except ValueError:
            return False

    def position(self, review_id):
        try:
            return self._entries.index(review_id)
        except ValueError:
            return -1

    def depth(self):
        return len(self._entries)

    def ids(self):
        return list(self._entries)

    def __len__(self):
        return len(self._entries)

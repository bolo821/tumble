"""BatchProcessor: buffer + flush by size/age."""
from __future__ import annotations


class BatchProcessor:
    def __init__(self, *, max_size, max_age_seconds):
        self._max_size = max_size
        self._max_age_seconds = max_age_seconds
        self._buffer = []
        self._first_at = None

    def add(self, *, item, at):
        if not self._buffer:
            self._first_at = at
        self._buffer.append(item)
        return self

    def should_flush(self, *, now):
        if len(self._buffer) >= self._max_size:
            return True
        if self._first_at is not None and (now - self._first_at) >= self._max_age_seconds:
            return True
        return False

    def flush(self):
        out = list(self._buffer)
        self._buffer = []
        self._first_at = None
        return out

    def depth(self):
        return len(self._buffer)

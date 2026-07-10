"""Counter: per-key monotonic counter."""
from __future__ import annotations


class Counter:
    def __init__(self):
        self._values = {}

    def inc(self, *, key, by=1):
        self._values[key] = self._values.get(key, 0) + by
        return self

    def get(self, key):
        return self._values.get(key, 0)

    def total(self):
        return sum(self._values.values())

    def keys(self):
        return sorted(self._values)

    def reset(self, key):
        self._values.pop(key, None)
        return self

    def __len__(self):
        return len(self._values)

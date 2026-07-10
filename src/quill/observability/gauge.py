"""Gauge: per-key settable value."""
from __future__ import annotations


class Gauge:
    def __init__(self):
        self._values = {}

    def set(self, *, key, value):
        self._values[key] = value
        return self

    def get(self, key):
        return self._values.get(key, 0)

    def inc(self, *, key, by=1):
        self._values[key] = self._values.get(key, 0) + by
        return self

    def dec(self, *, key, by=1):
        self._values[key] = self._values.get(key, 0) - by
        return self

    def keys(self):
        return sorted(self._values)

    def __len__(self):
        return len(self._values)

"""Histogram: bucketed value histogram."""
from __future__ import annotations


class Histogram:
    def __init__(self, *, buckets):
        self._buckets = sorted(buckets)
        self._counts = [0] * (len(self._buckets) + 1)
        self._sum = 0
        self._count = 0

    def observe(self, value):
        self._sum += value
        self._count += 1
        for i, b in enumerate(self._buckets):
            if value <= b:
                self._counts[i] += 1
                return self
        self._counts[-1] += 1
        return self

    def average(self):
        if self._count == 0:
            return 0
        return self._sum // self._count

    def total_count(self):
        return self._count

    def bucket_counts(self):
        return list(self._counts)

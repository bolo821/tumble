"""CacheStats: per-key hit/miss accounting."""
from __future__ import annotations


class CacheStats:
    def __init__(self):
        self._hits = 0
        self._misses = 0
        self._by_key = {}

    def record_hit(self, key):
        self._hits += 1
        bucket = self._by_key.setdefault(key, {"hits": 0, "misses": 0})
        bucket["hits"] += 1
        return self

    def record_miss(self, key):
        self._misses += 1
        bucket = self._by_key.setdefault(key, {"hits": 0, "misses": 0})
        bucket["misses"] += 1
        return self

    def hits(self):
        return self._hits

    def misses(self):
        return self._misses

    def hit_rate(self):
        total = self._hits + self._misses
        if total == 0:
            return 0.0
        return self._hits / total

    def hottest_keys(self, *, n=5):
        items = [(k, b["hits"]) for k, b in self._by_key.items()]
        items.sort(key=lambda kv: -kv[1])
        return items[:n]

"""CacheStore: in-memory TTL cache."""
from __future__ import annotations


class CacheStore:
    def __init__(self):
        self._entries = {}

    def put(self, *, key, value, size_bytes=0, expires_at=0):
        self._entries[key] = {
            "value": value, "size": size_bytes, "expires_at": expires_at,
        }
        return self

    def get(self, key):
        entry = self._entries.get(key, {})
        return entry.get("value")

    def has(self, key):
        return key in self._entries

    def size_of(self, key):
        return self._entries.get(key, {}).get("size", 0)

    def total_size(self):
        return sum(e["size"] for e in self._entries.values())

    def entry_count(self):
        return len(self._entries)

    def evict(self, key):
        self._entries.pop(key, None)
        return self

    def expired(self, *, now):
        return sorted(
            k for k, e in self._entries.items()
            if e.get("expires_at", 0) and now >= e["expires_at"]
        )

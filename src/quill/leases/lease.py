"""Lease: a bounded reservation."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class Lease:
    key: str
    holder: str
    granted_at: int
    expires_at: int

    def is_expired(self, *, now):
        return now >= self.expires_at

    def remaining(self, *, now):
        return max(0, self.expires_at - now)


class LeaseStore:
    def __init__(self):
        self._leases = {}

    def grant(self, *, key, holder, at, ttl_seconds):
        existing = self._leases.get(key)
        if existing is not None and not existing.is_expired(now=at):
            return None
        lease = Lease(key=key, holder=holder, granted_at=at,
                      expires_at=at + ttl_seconds)
        self._leases[key] = lease
        return lease

    def get(self, key):
        return self._leases.get(key)

    def release(self, key):
        self._leases.pop(key, None)
        return self

    def active(self, *, now):
        return [l for l in self._leases.values() if not l.is_expired(now=now)]

    def expired(self, *, now):
        return [l for l in self._leases.values() if l.is_expired(now=now)]

    def __len__(self):
        return len(self._leases)

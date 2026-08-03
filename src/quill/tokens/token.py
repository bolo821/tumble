"""Token: an API-scoped token."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class Token:
    id: str
    principal_id: str
    scope: str
    issued_at: int
    expires_at: int = 0

    def is_expired(self, *, now):
        return self.expires_at != 0 and now >= self.expires_at

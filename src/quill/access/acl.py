"""ACL: per-target permission entries."""
from __future__ import annotations

from dataclasses import dataclass


READ = "read"
WRITE = "write"
DELETE = "delete"
ADMIN = "admin"


@dataclass(frozen=True)
class AclEntry:
    principal_id: str
    target: str
    permission: str
    granted_at: int = 0
    expires_at: int = 0

    def is_expired(self, *, now):
        return self.expires_at != 0 and now >= self.expires_at


class Acl:
    def __init__(self):
        self._entries = []

    def grant(self, entry):
        self._entries.append(entry)
        return self

    def revoke(self, *, principal_id, target, permission):
        self._entries = [
            e for e in self._entries
            if not (e.principal_id == principal_id and e.target == target
                    and e.permission == permission)
        ]
        return self

    def entries(self):
        return list(self._entries)

    def for_target(self, target):
        return [e for e in self._entries if e.target == target]

    def for_principal(self, principal_id):
        return [e for e in self._entries if e.principal_id == principal_id]

    def __len__(self):
        return len(self._entries)

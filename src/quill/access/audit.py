"""audit: log access decisions."""
from __future__ import annotations


class AccessAuditLog:
    def __init__(self):
        self._events = []

    def record(self, *, principal_id, target, permission, decision, at):
        self._events.append({
            "principal_id": principal_id,
            "target": target,
            "permission": permission,
            "decision": decision,
            "at": at,
        })
        return self

    def events(self):
        return list(self._events)

    def for_principal(self, principal_id):
        return [e for e in self._events if e["principal_id"] == principal_id]

    def denials(self):
        return [e for e in self._events if e["decision"] == "deny"]

    def grants(self):
        return [e for e in self._events if e["decision"] == "grant"]

    def __len__(self):
        return len(self._events)

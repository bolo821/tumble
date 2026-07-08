"""AuditLog: append-only event log."""
from __future__ import annotations


class AuditLog:
    def __init__(self):
        self._events = []

    def record(self, *, at, actor, kind, target, payload=None):
        self._events.append({
            "at": at, "actor": actor, "kind": kind,
            "target": target, "payload": payload or {},
        })
        return self

    def events(self):
        return list(self._events)

    def for_actor(self, actor):
        return [e for e in self._events if e["actor"] == actor]

    def for_target(self, target):
        return [e for e in self._events if e["target"] == target]

    def of_kind(self, kind):
        return [e for e in self._events if e["kind"] == kind]

    def __len__(self):
        return len(self._events)

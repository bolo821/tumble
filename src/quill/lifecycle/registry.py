"""LifecycleRegistry: wire coordinator + observer + audit."""
from __future__ import annotations


class LifecycleRegistry:
    def __init__(self):
        self._audits = []
        self._observers = []

    def register_audit(self, audit):
        self._audits.append(audit)
        return self

    def register_observer(self, observer):
        self._observers.append(observer)
        return self

    def audits(self):
        return list(self._audits)

    def observers(self):
        return list(self._observers)

    def broadcast_transition(self, *, review_id, from_status, to_status, at, actor=""):
        for a in self._audits:
            a.record(review_id=review_id, from_status=from_status,
                     to_status=to_status, at=at, actor=actor)
        for o in self._observers:
            if hasattr(o, "record_status"):
                o.record_status(review_id=review_id, from_status=from_status,
                                to_status=to_status, at=at)
        return self

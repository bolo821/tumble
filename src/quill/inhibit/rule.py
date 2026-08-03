"""InhibitRule: suppress a downstream notification when a matching parent exists."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class InhibitRule:
    parent_event: str
    child_event: str
    within_seconds: int = 3600


class InhibitStore:
    def __init__(self):
        self._rules = []

    def add(self, rule):
        self._rules.append(rule)
        return self

    def all(self):
        return list(self._rules)

    def for_parent(self, parent_event):
        return [r for r in self._rules if r.parent_event == parent_event]


def is_inhibited(*, store, parent_events, child_event, at):
    for rule in store.all():
        if rule.child_event != child_event:
            continue
        threshold = at - rule.within_seconds
        for parent_at, kind in parent_events:
            if kind == rule.parent_event and parent_at >= threshold:
                return True
    return False

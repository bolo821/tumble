"""Channel: a notification destination."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class Channel:
    name: str
    kind: str  # email | slack | webhook
    target: str
    on_open: bool = True
    on_approve: bool = True
    on_merge: bool = True
    on_close: bool = False

    def applies_to(self, event_kind):
        if event_kind == "open":
            return self.on_open
        if event_kind == "approve":
            return self.on_approve
        if event_kind == "merge":
            return self.on_merge
        if event_kind == "close":
            return self.on_close
        return False

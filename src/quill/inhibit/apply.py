"""apply: apply inhibition to a notification stream."""
from __future__ import annotations

from quill.inhibit.rule import is_inhibited


def filter_notifications(*, store, notifications, parent_events):
    out = []
    for n in notifications:
        if is_inhibited(store=store, parent_events=parent_events,
                        child_event=n.get("kind", ""), at=n.get("at", 0)):
            continue
        out.append(n)
    return out


def inhibited_count(*, store, notifications, parent_events):
    count = 0
    for n in notifications:
        if is_inhibited(store=store, parent_events=parent_events,
                        child_event=n.get("kind", ""), at=n.get("at", 0)):
            count += 1
    return count

"""replay: replay dispatcher state."""
from __future__ import annotations


def replay_dispatched(dispatcher, *, sink):
    for e in dispatcher._sent:
        sink(e)
    return dispatcher


def total_for_event(dispatcher, *, event_kind):
    return sum(1 for e in dispatcher._sent if e["event"] == event_kind)


def reviews_notified(dispatcher):
    return sorted({e["review_id"] for e in dispatcher._sent})


def channels_used(dispatcher):
    return sorted({e["channel"] for e in dispatcher._sent})

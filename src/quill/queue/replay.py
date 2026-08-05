"""replay: replay QueueObserver events into a sink."""
from __future__ import annotations


def replay(observer, *, sink):
    for e in observer._events:
        sink(e)
    return observer


def replay_kind(observer, *, kind, sink):
    for e in observer._events:
        if e["kind"] == kind:
            sink(e)
    return observer

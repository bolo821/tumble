"""replay: replay AuditLog events into a sink."""
from __future__ import annotations


def replay(log, *, sink):
    for e in log.events():
        sink(e)
    return log


def replay_window(log, *, sink, start, end):
    for e in log.events():
        if start <= e["at"] < end:
            sink(e)
    return log


def replay_of_kind(log, *, kind, sink):
    for e in log.events():
        if e["kind"] == kind:
            sink(e)
    return log

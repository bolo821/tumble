"""replay: stream events through a sink."""
from __future__ import annotations


def replay_all(stream, *, sink):
    for e in stream.events():
        sink(e)
    return stream


def replay_window(stream, *, sink, start, end):
    for e in stream.in_window(start=start, end=end):
        sink(e)
    return stream


def replay_of_kind(stream, *, kind, sink):
    for e in stream.of_kind(kind):
        sink(e)
    return stream

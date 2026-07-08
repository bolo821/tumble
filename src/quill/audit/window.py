"""window: time-windowed audit views."""
from __future__ import annotations


def in_window(log, *, start, end):
    return [e for e in log.events() if start <= e["at"] < end]


def actors_in_window(log, *, start, end):
    return sorted({e["actor"] for e in in_window(log, start=start, end=end)})


def count_in_window(log, *, start, end):
    return len(in_window(log, start=start, end=end))


def busiest_window(log, *, window_size_seconds, n_windows=12):
    if len(log) == 0:
        return None
    events = sorted(log.events(), key=lambda e: e["at"])
    start = events[0]["at"]
    counts = [0] * n_windows
    for e in events:
        idx = (e["at"] - start) // window_size_seconds
        if 0 <= idx < n_windows:
            counts[idx] += 1
    if max(counts) == 0:
        return None
    return counts.index(max(counts))

"""sliding: sliding-window accounting."""
from __future__ import annotations


def trim_window(events, *, now, window_seconds):
    threshold = now - window_seconds
    return [t for t in events if t > threshold]


def count_in_window(events, *, now, window_seconds):
    return len(trim_window(events, now=now, window_seconds=window_seconds))


def oldest(events, *, now, window_seconds):
    inside = trim_window(events, now=now, window_seconds=window_seconds)
    return min(inside) if inside else None

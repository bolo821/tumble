"""calendar: maintenance calendar helpers."""
from __future__ import annotations


def windows_in_range(store, *, start, end):
    return [w for w in store.all() if w.starts_at < end and w.ends_at > start]


def total_maintenance_seconds(store, *, start, end):
    out = 0
    for w in windows_in_range(store, start=start, end=end):
        out += max(0, min(w.ends_at, end) - max(w.starts_at, start))
    return out


def next_window_after(store, *, at):
    upcoming = [w for w in store.all() if w.starts_at > at]
    if not upcoming:
        return None
    upcoming.sort(key=lambda w: w.starts_at)
    return upcoming[0]

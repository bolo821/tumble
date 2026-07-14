"""trend: per-window trends."""
from __future__ import annotations


def per_window_count(reviews, *, window_seconds, n_windows):
    if not reviews:
        return [0] * n_windows
    items = sorted(reviews, key=lambda r: r.created_at)
    start = items[0].created_at
    counts = [0] * n_windows
    for r in items:
        idx = (r.created_at - start) // window_seconds
        if 0 <= idx < n_windows:
            counts[idx] += 1
    return counts


def peak_window(counts):
    if not counts or max(counts) == 0:
        return None
    return counts.index(max(counts))


def quiet_window(counts):
    if not counts or all(c == 0 for c in counts):
        return None
    return counts.index(min(counts))


def average(counts):
    if not counts:
        return 0
    return sum(counts) // len(counts)

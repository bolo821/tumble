"""throughput_report: merged-per-window throughput."""
from __future__ import annotations


def merged_per_window(store, *, window_seconds, n_windows):
    merged = [r for r in store.all() if r.is_merged()]
    if not merged:
        return [0] * n_windows
    items = sorted(merged, key=lambda r: r.updated_at)
    start = items[0].updated_at
    counts = [0] * n_windows
    for r in items:
        idx = (r.updated_at - start) // window_seconds
        if 0 <= idx < n_windows:
            counts[idx] += 1
    return counts


def peak(counts):
    if not counts or max(counts) == 0:
        return None
    return counts.index(max(counts))


def average(counts):
    if not counts:
        return 0
    return sum(counts) // len(counts)

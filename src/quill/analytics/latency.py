"""latency: open-to-merge latency."""
from __future__ import annotations


def merge_latencies(reviews):
    out = []
    for r in reviews:
        if r.is_merged() and r.created_at:
            out.append(max(0, r.updated_at - r.created_at))
    return out


def average(values):
    if not values:
        return 0
    return sum(values) // len(values)


def median(values):
    if not values:
        return 0
    items = sorted(values)
    mid = len(items) // 2
    if len(items) % 2 == 0:
        return (items[mid - 1] + items[mid]) // 2
    return items[mid]


def percentile(values, *, p):
    if not values:
        return 0
    items = sorted(values)
    idx = max(0, min(len(items) - 1, int(p * len(items) / 100)))
    return items[idx]

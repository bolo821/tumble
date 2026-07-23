"""signals: derived quill signals."""
from __future__ import annotations


def merges_per_day(reviews, *, window_seconds=86400):
    if not reviews:
        return 0
    items = sorted([r for r in reviews if r.is_merged()],
                   key=lambda r: r.updated_at)
    if not items:
        return 0
    start = items[0].updated_at
    end = items[-1].updated_at + 1
    span = max(end - start, 1)
    days = max(1, span // window_seconds)
    return len(items) // days


def open_velocity(reviews, *, window_seconds=86400, now=0):
    threshold = now - window_seconds
    return sum(1 for r in reviews
               if r.is_open() and r.created_at >= threshold)


def stale_reviews(reviews, *, max_age_seconds, now):
    out = []
    for r in reviews:
        if not r.is_open():
            continue
        if now - r.updated_at > max_age_seconds:
            out.append(r)
    return out

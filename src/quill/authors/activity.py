"""activity: per-author activity window."""
from __future__ import annotations


def per_window(store, *, author, window_seconds, n_windows):
    reviews = [r for r in store.all() if r.author == author]
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


def days_active(store, *, author):
    reviews = [r for r in store.all() if r.author == author]
    days = {r.created_at // 86400 for r in reviews if r.created_at}
    return len(days)


def first_review_at(store, *, author):
    reviews = [r for r in store.all() if r.author == author]
    if not reviews:
        return None
    return min(r.created_at for r in reviews)


def last_review_at(store, *, author):
    reviews = [r for r in store.all() if r.author == author]
    if not reviews:
        return None
    return max(r.updated_at for r in reviews)

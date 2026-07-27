"""series: time-series helpers."""
from __future__ import annotations


def status_count_series(store, *, status):
    out = []
    for s in store.all():
        count = sum(1 for r in s.review_payload if r["status"] == status)
        out.append((s.at, count))
    return out


def total_count_series(store):
    return [(s.at, s.review_count()) for s in store.all()]


def peak_count_time(store):
    if len(store) == 0:
        return None
    return max(store.all(), key=lambda s: s.review_count()).at


def first_time_status_reaches(store, *, status, threshold):
    for s in store.all():
        count = sum(1 for r in s.review_payload if r["status"] == status)
        if count >= threshold:
            return s.at
    return None

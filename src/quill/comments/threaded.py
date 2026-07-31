"""threaded: group comments by thread."""
from __future__ import annotations


def group_by_thread(store, review_id):
    out = {}
    for c in store.for_review(review_id):
        out.setdefault(c.thread_id, []).append(c)
    return out


def thread_size(store, thread_id):
    return len(store.for_thread(thread_id))


def largest_threads(store, review_id, *, n=5):
    grouped = group_by_thread(store, review_id)
    sized = [(tid, len(cs)) for tid, cs in grouped.items()]
    sized.sort(key=lambda kv: -kv[1])
    return sized[:n]


def first_in_thread(store, thread_id):
    items = store.for_thread(thread_id)
    if not items:
        return None
    return min(items, key=lambda c: c.created_at)


def latest_in_thread(store, thread_id):
    items = store.for_thread(thread_id)
    if not items:
        return None
    return max(items, key=lambda c: c.created_at)

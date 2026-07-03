"""resolver: mark threads resolved / unresolved."""
from __future__ import annotations


def resolve(thread, *, at=0):
    thread.resolved = True
    return thread


def unresolve(thread, *, at=0):
    thread.resolved = False
    return thread


def resolve_all_for_review(store, *, review_id):
    changed = []
    for t in store.for_review(review_id):
        if not t.resolved:
            resolve(t)
            changed.append(t.id)
    return changed


def unresolved_count(store, review_id):
    return len(store.unresolved(review_id))


def has_unresolved(store, review_id):
    return unresolved_count(store, review_id) > 0

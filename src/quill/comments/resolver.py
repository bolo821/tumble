"""resolver: mark individual comments resolved."""
from __future__ import annotations


def resolve(comment):
    comment.resolved = True
    return comment


def unresolve(comment):
    comment.resolved = False
    return comment


def resolve_thread(store, *, thread_id):
    changed = []
    for c in store.for_thread(thread_id):
        if not c.resolved:
            resolve(c)
            changed.append(c.id)
    return changed


def unresolved_for_review(store, review_id):
    return [c for c in store.for_review(review_id) if not c.resolved]

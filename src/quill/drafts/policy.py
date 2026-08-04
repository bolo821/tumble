"""policy: draft admission rules."""
from __future__ import annotations


def is_over_limit(store, *, author, cap):
    return len(store.for_author(author)) >= cap


def can_stash(store, *, author, cap):
    return not is_over_limit(store, author=author, cap=cap)


def remaining_capacity(store, *, author, cap):
    return max(0, cap - len(store.for_author(author)))

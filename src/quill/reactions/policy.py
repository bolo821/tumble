"""policy: reaction rate-limit / dedupe."""
from __future__ import annotations


def has_already_reacted(store, *, target_id, reviewer_id, emoji):
    for r in store.for_target(target_id):
        if r.reviewer_id == reviewer_id and r.emoji == emoji:
            return True
    return False


def can_react(store, *, target_id, reviewer_id, emoji, max_per_target=10):
    if has_already_reacted(store, target_id=target_id,
                            reviewer_id=reviewer_id, emoji=emoji):
        return False, "already-reacted"
    if len(store.for_target(target_id)) >= max_per_target:
        return False, "target-full"
    return True, "allowed"


def reactions_by(store, *, target_id, reviewer_id):
    return [r for r in store.for_target(target_id) if r.reviewer_id == reviewer_id]

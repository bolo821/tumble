"""decider: retention decisions."""
from __future__ import annotations


def must_expire(*, review, registry, now):
    for label in review.labels:
        policy = registry.get(label)
        if policy is None or policy.keep_seconds <= 0:
            continue
        if now >= policy.must_expire_by(created_at=review.created_at):
            return True
    return False


def time_to_expire(*, review, registry, now):
    times = []
    for label in review.labels:
        policy = registry.get(label)
        if policy is None or policy.keep_seconds <= 0:
            continue
        times.append(max(0, policy.must_expire_by(created_at=review.created_at) - now))
    return min(times) if times else None

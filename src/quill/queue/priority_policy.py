"""priority_policy: derive priority for a Review."""
from __future__ import annotations


def base_priority(review):
    """Lower is higher priority."""
    priority = 100
    if review.is_approved():
        priority -= 50
    if any(l == "urgent" for l in review.labels):
        priority -= 30
    if any(l == "hotfix" for l in review.labels):
        priority -= 40
    return priority


def by_labels(review, *, weights):
    priority = 100
    for label in review.labels:
        priority -= weights.get(label, 0)
    return priority


def by_age(review, *, now, boost_per_hour=1):
    age_hours = max(0, (now - review.created_at) // 3600)
    return 100 - int(boost_per_hour * age_hours)

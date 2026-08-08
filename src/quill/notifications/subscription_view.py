"""subscription_view: convenience over SubscriptionStore."""
from __future__ import annotations


def subscribers_for_review(store, review_id):
    return sorted({s.reviewer_id for s in store._subs if s.review_id == review_id})


def subscribers_for_label(store, label):
    return sorted({s.reviewer_id for s in store._subs if s.label == label})


def per_reviewer_subscription_count(store):
    out = {}
    for s in store._subs:
        out[s.reviewer_id] = out.get(s.reviewer_id, 0) + 1
    return out


def most_subscribed_reviewers(store, *, n=5):
    counts = per_reviewer_subscription_count(store)
    return sorted(counts.items(), key=lambda kv: -kv[1])[:n]

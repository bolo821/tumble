"""policy: filter notifications by review attributes."""
from __future__ import annotations


def by_author_only(*, allowed_authors):
    def matches(review):
        return review.author in set(allowed_authors)
    return matches


def by_label_only(*, allowed_labels):
    def matches(review):
        return bool(review.label_set() & set(allowed_labels))
    return matches


def by_open_only(review):
    return review.is_open()


def evaluate(*, channel, review, event_kind, predicates):
    if not channel.applies_to(event_kind):
        return False
    return all(p(review) for p in predicates)

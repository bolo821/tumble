"""matcher: evaluate a Query against a Review."""
from __future__ import annotations


def matches_title(review, q):
    if not q.title_substring:
        return True
    return q.title_substring.lower() in review.title.lower()


def matches_authors(review, q):
    if not q.authors:
        return True
    return review.author in set(q.authors)


def matches_labels_all(review, q):
    if not q.labels_all:
        return True
    return set(q.labels_all) <= review.label_set()


def matches_labels_any(review, q):
    if not q.labels_any:
        return True
    return bool(set(q.labels_any) & review.label_set())


def matches_statuses(review, q):
    if not q.statuses:
        return True
    return review.status in set(q.statuses)


def matches_files(review, q):
    if not q.files:
        return True
    return bool(set(q.files) & set(review.changed_files))


def matches(review, q):
    return (
        matches_title(review, q)
        and matches_authors(review, q)
        and matches_labels_all(review, q)
        and matches_labels_any(review, q)
        and matches_statuses(review, q)
        and matches_files(review, q)
    )


def filter_reviews(reviews, q):
    return [r for r in reviews if matches(r, q)]

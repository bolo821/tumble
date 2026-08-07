"""labels: helpers for label tuples on reviews."""
from __future__ import annotations


def add_label(review, label):
    if label in review.labels:
        return False
    review.labels = tuple(list(review.labels) + [label])
    return True


def remove_label(review, label):
    if label not in review.labels:
        return False
    review.labels = tuple(l for l in review.labels if l != label)
    return True


def has_all(review, labels):
    return set(labels) <= review.label_set()


def has_any(review, labels):
    return bool(set(labels) & review.label_set())


def toggle(review, label):
    if label in review.labels:
        remove_label(review, label)
    else:
        add_label(review, label)
    return review

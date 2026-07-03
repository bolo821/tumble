"""LabelIndex: per-label review-id index."""
from __future__ import annotations


class LabelIndex:
    def __init__(self):
        self._by_label = {}

    def add(self, review):
        for label in review.label_set():
            self._by_label.setdefault(label, set()).add(review.id)
        return self

    def remove(self, review):
        for label in review.label_set():
            self._by_label.get(label, set()).discard(review.id)
        return self

    def find(self, label):
        return sorted(self._by_label.get(label, set()))

    def count_for(self, label):
        return len(self._by_label.get(label, set()))

    def labels(self):
        return sorted(self._by_label)


def reviews_with_all(index, labels):
    if not labels:
        return []
    sets = [set(index.find(l)) for l in labels]
    return sorted(set.intersection(*sets)) if sets else []


def reviews_with_any(index, labels):
    out = set()
    for l in labels:
        out |= set(index.find(l))
    return sorted(out)

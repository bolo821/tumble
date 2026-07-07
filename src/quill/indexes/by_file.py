"""ByFile: reviews grouped by changed file path."""
from __future__ import annotations


class ByFile:
    def __init__(self):
        self._by_path = {}

    def add(self, review):
        for path in review.changed_files:
            self._by_path.setdefault(path, set()).add(review.id)
        return self

    def remove(self, review):
        for path in review.changed_files:
            self._by_path.get(path, set()).discard(review.id)
        return self

    def find(self, path):
        return sorted(self._by_path.get(path, set()))

    def count_for(self, path):
        return len(self._by_path.get(path, set()))

    def paths(self):
        return sorted(self._by_path)

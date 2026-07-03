"""LabelRegistry: per-name label registry."""
from __future__ import annotations


class LabelRegistry:
    def __init__(self):
        self._by_name = {}

    def register(self, label):
        self._by_name[label.name] = label
        return self

    def get(self, name):
        return self._by_name.get(name)

    def all(self):
        return list(self._by_name.values())

    def names(self):
        return sorted(self._by_name)

    def remove(self, name):
        self._by_name.pop(name, None)
        return self

    def with_color(self, color):
        return [l for l in self._by_name.values() if l.color == color]

    def __len__(self):
        return len(self._by_name)

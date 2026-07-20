"""LockStore: per-name exclusive locks."""
from __future__ import annotations


class LockStore:
    def __init__(self):
        self._held = {}

    def try_acquire(self, *, name, holder):
        if name in self._held and self._held[name] != holder:
            return False
        self._held[name] = holder
        return True

    def release(self, *, name, holder):
        if self._held.get(name) == holder:
            self._held.pop(name, None)
            return True
        return False

    def holder(self, name):
        return self._held.get(name)

    def is_held(self, name):
        return name in self._held

    def held_names(self):
        return sorted(self._held)

    def __len__(self):
        return len(self._held)

"""ReconcileLoop: thin driver."""
from __future__ import annotations


class ReconcileLoop:
    def __init__(self):
        self._iterations = 0
        self._last_diff = []

    def reconcile(self, *, desired, actual, diff_fn, apply_fn):
        diff = diff_fn(desired, actual)
        self._last_diff = list(diff)
        for action in diff:
            apply_fn(action)
        self._iterations += 1
        return diff

    def iterations(self):
        return self._iterations

    def last_diff(self):
        return list(self._last_diff)


def is_converged(diff):
    return not diff

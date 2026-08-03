"""GateState: open/closed gates by name."""
from __future__ import annotations


class GateState:
    def __init__(self):
        self._open = set()

    def open(self, name):
        self._open.add(name)
        return self

    def close(self, name):
        self._open.discard(name)
        return self

    def is_open(self, name):
        return name in self._open

    def all_open(self):
        return sorted(self._open)

    def __len__(self):
        return len(self._open)


def open_all(state, names):
    for n in names:
        state.open(n)
    return state


def close_all(state, names):
    for n in names:
        state.close(n)
    return state

"""EnvLayer: scoped env variable resolution."""
from __future__ import annotations


class EnvLayer:
    def __init__(self):
        self._layers = []

    def push(self, *, scope, values):
        self._layers.append((scope, dict(values)))
        return self

    def pop(self):
        if self._layers:
            self._layers.pop()
        return self

    def get(self, key):
        for _, values in reversed(self._layers):
            if key in values:
                return values[key]
        return None

    def has(self, key):
        return self.get(key) is not None

    def flatten(self):
        out = {}
        for _, values in self._layers:
            out.update(values)
        return out

    def depth(self):
        return len(self._layers)

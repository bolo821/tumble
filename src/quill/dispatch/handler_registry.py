"""HandlerRegistry: register handlers by name."""
from __future__ import annotations


class HandlerRegistry:
    def __init__(self):
        self._by_name = {}

    def register(self, name, handler):
        self._by_name[name] = handler
        return self

    def get(self, name):
        return self._by_name.get(name)

    def names(self):
        return sorted(self._by_name)

    def dispatch(self, name, payload):
        h = self._by_name.get(name)
        if h is None:
            return None
        return h(payload)

    def __len__(self):
        return len(self._by_name)

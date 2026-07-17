"""pipeline: dispatch middleware chain."""
from __future__ import annotations


class DispatchPipeline:
    def __init__(self):
        self._middlewares = []

    def use(self, middleware):
        self._middlewares.append(middleware)
        return self

    def dispatch(self, item):
        for mw in self._middlewares:
            item = mw(item)
            if item is None:
                return None
        return item

    def __len__(self):
        return len(self._middlewares)


def with_logging(log):
    def mw(item):
        log.append(item)
        return item
    return mw


def with_filter(predicate):
    def mw(item):
        return item if predicate(item) else None
    return mw

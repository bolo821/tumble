"""DependencyGraph: edges between review ids."""
from __future__ import annotations


class DependencyGraph:
    def __init__(self):
        self._edges = {}

    def add_edge(self, *, source, target):
        self._edges.setdefault(source, set()).add(target)
        return self

    def edges(self):
        out = []
        for src, targets in self._edges.items():
            for t in sorted(targets):
                out.append((src, t))
        return sorted(out)

    def successors(self, source):
        return sorted(self._edges.get(source, set()))

    def predecessors(self, target):
        return sorted(s for s, ts in self._edges.items() if target in ts)

    def nodes(self):
        out = set(self._edges)
        for ts in self._edges.values():
            out |= ts
        return sorted(out)

    def __len__(self):
        return sum(len(ts) for ts in self._edges.values())

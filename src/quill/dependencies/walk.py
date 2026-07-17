"""walk: DFS over a DependencyGraph."""
from __future__ import annotations


def descendants(graph, source):
    out = set()
    stack = list(graph.successors(source))
    while stack:
        n = stack.pop()
        if n in out:
            continue
        out.add(n)
        stack.extend(graph.successors(n))
    return sorted(out)


def ancestors(graph, target):
    out = set()
    stack = list(graph.predecessors(target))
    while stack:
        n = stack.pop()
        if n in out:
            continue
        out.add(n)
        stack.extend(graph.predecessors(n))
    return sorted(out)


def is_reachable(graph, *, source, target):
    return target in set(descendants(graph, source))


def fanout(graph, source):
    return len(graph.successors(source))


def fanin(graph, target):
    return len(graph.predecessors(target))

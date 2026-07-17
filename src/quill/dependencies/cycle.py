"""cycle: detect cycles + topological sort."""
from __future__ import annotations


def has_cycle(graph):
    WHITE, GREY, BLACK = 0, 1, 2
    color = {n: WHITE for n in graph.nodes()}

    def visit(n):
        color[n] = GREY
        for nxt in graph.successors(n):
            if nxt not in color:
                color[nxt] = WHITE
            if color[nxt] == GREY:
                return True
            if color[nxt] == WHITE:
                if visit(nxt):
                    return True
        color[n] = BLACK
        return False

    for n in list(color):
        if color[n] == WHITE:
            if visit(n):
                return True
    return False


def topological_order(graph):
    incoming = {n: 0 for n in graph.nodes()}
    for src, tgt in graph.edges():
        incoming[tgt] = incoming.get(tgt, 0) + 1
    ready = sorted([n for n in incoming if incoming[n] == 0])
    out = []
    while ready:
        n = ready.pop(0)
        out.append(n)
        for nxt in graph.successors(n):
            incoming[nxt] -= 1
            if incoming[nxt] == 0:
                ready.append(nxt)
                ready.sort()
    return out

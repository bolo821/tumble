"""fairness: round-robin fairness."""
from __future__ import annotations


def round_robin(items, *, last_index):
    if not items:
        return None, -1
    idx = (last_index + 1) % len(items)
    return items[idx], idx


def fair_pick(*, candidates, last_picked_at):
    if not candidates:
        return None
    return min(candidates, key=lambda c: last_picked_at.get(c, 0))


def update_pick_time(state, *, picked, at):
    state[picked] = at
    return state

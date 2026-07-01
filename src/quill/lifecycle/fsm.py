"""Review transition table helpers."""
from __future__ import annotations

from quill.core.status import ReviewStatus


_TRANSITIONS = {
    ReviewStatus.DRAFT: {ReviewStatus.OPEN, ReviewStatus.CLOSED},
    ReviewStatus.OPEN: {ReviewStatus.APPROVED, ReviewStatus.DRAFT, ReviewStatus.CLOSED},
    ReviewStatus.APPROVED: {ReviewStatus.OPEN, ReviewStatus.MERGED, ReviewStatus.CLOSED},
}


def can_transition(*, from_status, to_status):
    return to_status in _TRANSITIONS.get(from_status, set())


def legal_targets(from_status):
    return sorted(_TRANSITIONS.get(from_status, set()))


def is_terminal(status):
    return ReviewStatus.is_terminal(status)


def all_reachable(start):
    seen = {start}
    stack = [start]
    while stack:
        cur = stack.pop()
        for nxt in _TRANSITIONS.get(cur, set()):
            if nxt in seen:
                continue
            seen.add(nxt)
            stack.append(nxt)
    return sorted(seen)

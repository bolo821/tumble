"""transitions: apply a transition to a Review."""
from __future__ import annotations

from quill.core.status import ReviewStatus
from quill.errors import InvalidTransition
from quill.lifecycle.fsm import can_transition


def transition(review, *, to, at):
    if not can_transition(from_status=review.status, to_status=to):
        raise InvalidTransition(review.status + "->" + to)
    review.status = to
    review.updated_at = at
    return review


def can(review, *, to):
    return can_transition(from_status=review.status, to_status=to)


def reachable_targets(review):
    from quill.lifecycle.fsm import legal_targets
    return legal_targets(review.status)

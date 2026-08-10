from quill.core.status import ReviewStatus
from quill.lifecycle.fsm import can_transition, legal_targets, is_terminal


def test_draft_can_open():
    assert can_transition(from_status=ReviewStatus.DRAFT,
                          to_status=ReviewStatus.OPEN)


def test_draft_cannot_merge():
    assert not can_transition(from_status=ReviewStatus.DRAFT,
                              to_status=ReviewStatus.MERGED)


def test_open_can_approve():
    assert can_transition(from_status=ReviewStatus.OPEN,
                          to_status=ReviewStatus.APPROVED)


def test_approved_can_merge():
    assert can_transition(from_status=ReviewStatus.APPROVED,
                          to_status=ReviewStatus.MERGED)


def test_terminal_no_targets():
    assert is_terminal(ReviewStatus.MERGED)
    assert legal_targets(ReviewStatus.MERGED) == []

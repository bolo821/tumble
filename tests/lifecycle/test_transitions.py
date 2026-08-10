import pytest

from quill.core.review import Review
from quill.core.status import ReviewStatus
from quill.lifecycle.transitions import transition, can
from quill.errors import InvalidTransition


def test_draft_to_open():
    r = Review(id="r", title="t", author="a")
    transition(r, to=ReviewStatus.OPEN, at=10)
    assert r.status == ReviewStatus.OPEN
    assert r.updated_at == 10


def test_invalid_transition_raises():
    r = Review(id="r", title="t", author="a")
    with pytest.raises(InvalidTransition):
        transition(r, to=ReviewStatus.MERGED, at=10)


def test_can_predicate():
    r = Review(id="r", title="t", author="a", status=ReviewStatus.APPROVED)
    assert can(r, to=ReviewStatus.MERGED)
    assert not can(r, to=ReviewStatus.DRAFT)

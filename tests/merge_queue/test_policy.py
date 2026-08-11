from quill.core.review import Review
from quill.core.status import ReviewStatus
from quill.merge_queue.policy import is_admittable, admit_reason


def test_admittable_when_all_green():
    r = Review(id="r", title="t", author="a",
               status=ReviewStatus.APPROVED)
    assert is_admittable(r, is_approved=True, ci_green=True,
                         has_conflicts=False)


def test_not_admittable_when_not_approved():
    r = Review(id="r", title="t", author="a",
               status=ReviewStatus.OPEN)
    assert not is_admittable(r, is_approved=False, ci_green=True,
                              has_conflicts=False)


def test_not_admittable_when_ci_red():
    r = Review(id="r", title="t", author="a",
               status=ReviewStatus.APPROVED)
    assert not is_admittable(r, is_approved=True, ci_green=False,
                              has_conflicts=False)


def test_reason_conflicts():
    reason = admit_reason(is_approved=True, ci_green=True,
                          has_conflicts=True)
    assert reason == "conflicts"


def test_reason_admittable():
    assert admit_reason(is_approved=True, ci_green=True,
                        has_conflicts=False) == "admittable"

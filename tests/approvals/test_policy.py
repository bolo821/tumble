from quill.approvals.approval import Approval, APPROVE, REQUEST_CHANGES
from quill.approvals.log import ApprovalLog
from quill.approvals.policy import (
    is_satisfied, has_blocking_change_request,
    approver_set, pending_count,
)


def _log(*entries):
    log = ApprovalLog()
    for e in entries:
        log.record(e)
    return log


def test_is_satisfied_needs_count():
    log = _log(Approval(review_id="r", reviewer="alice",
                        decision=APPROVE, at=1))
    assert not is_satisfied(log=log, review_id="r", required_count=2)
    assert is_satisfied(log=log, review_id="r", required_count=1)


def test_blocked_by_change_request():
    log = _log(
        Approval(review_id="r", reviewer="alice", decision=APPROVE, at=1),
        Approval(review_id="r", reviewer="bob",
                 decision=REQUEST_CHANGES, at=2),
    )
    assert has_blocking_change_request(log=log, review_id="r")
    assert not is_satisfied(log=log, review_id="r", required_count=1)


def test_latest_decision_wins():
    log = _log(
        Approval(review_id="r", reviewer="alice", decision=APPROVE, at=1),
        Approval(review_id="r", reviewer="alice",
                 decision=REQUEST_CHANGES, at=2),
    )
    assert has_blocking_change_request(log=log, review_id="r")


def test_required_reviewers():
    log = _log(Approval(review_id="r", reviewer="alice",
                        decision=APPROVE, at=1))
    assert is_satisfied(log=log, review_id="r", required_count=1,
                        required_reviewers=("alice",))
    assert not is_satisfied(log=log, review_id="r", required_count=1,
                             required_reviewers=("bob",))


def test_pending_count():
    log = _log(Approval(review_id="r", reviewer="alice",
                        decision=APPROVE, at=1))
    assert pending_count(log=log, review_id="r", required_count=3) == 2


def test_approver_set():
    log = _log(
        Approval(review_id="r", reviewer="alice", decision=APPROVE, at=1),
        Approval(review_id="r", reviewer="bob", decision=APPROVE, at=2),
    )
    assert approver_set(log=log, review_id="r") == {"alice", "bob"}

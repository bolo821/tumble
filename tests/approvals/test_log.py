from quill.approvals.approval import Approval, APPROVE
from quill.approvals.log import ApprovalLog


def test_record_and_query():
    log = ApprovalLog()
    log.record(Approval(review_id="r1", reviewer="alice",
                        decision=APPROVE, at=1))
    log.record(Approval(review_id="r1", reviewer="bob",
                        decision=APPROVE, at=2))
    assert len(log) == 2
    assert len(log.for_review("r1")) == 2
    assert len(log.by_reviewer("alice")) == 1


def test_latest_for():
    log = ApprovalLog()
    log.record(Approval(review_id="r1", reviewer="alice",
                        decision=APPROVE, at=1))
    log.record(Approval(review_id="r1", reviewer="alice",
                        decision="request_changes", at=2))
    latest = log.latest_for(review_id="r1", reviewer="alice")
    assert latest.decision == "request_changes"

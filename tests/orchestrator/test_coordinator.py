from quill.core.review import Review
from quill.core.status import ReviewStatus
from quill.core.store import ReviewStore
from quill.audit.log import AuditLog
from quill.observers.review_observer import ReviewObserver
from quill.orchestrator.coordinator import Coordinator


def _wire():
    return Coordinator(
        store=ReviewStore(),
        audit_log=AuditLog(),
        observer=ReviewObserver(),
    )


def test_full_lifecycle():
    c = _wire()
    r = Review(id="r", title="t", author="alice")
    c.create(review=r, at=1, actor="alice")
    c.open(review_id="r", at=2, actor="alice")
    c.approve(review_id="r", at=3, actor="bob")
    c.merge(review_id="r", at=4, actor="bob")
    assert r.status == ReviewStatus.MERGED
    assert r.updated_at == 4


def test_audit_records_each_transition():
    c = _wire()
    r = Review(id="r", title="t", author="alice")
    c.create(review=r, at=1, actor="alice")
    c.open(review_id="r", at=2, actor="alice")
    assert len(c._audit) == 2
    assert c._audit.of_kind("review.create")[0]["target"] == "r"

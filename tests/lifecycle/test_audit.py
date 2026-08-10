from quill.lifecycle.audit import TransitionAudit


def test_records_and_queries():
    audit = TransitionAudit()
    audit.record(review_id="r1", from_status="draft",
                 to_status="open", at=10, actor="alice")
    assert len(audit) == 1
    assert audit.for_review("r1")[0]["to"] == "open"
    assert len(audit.by_actor("alice")) == 1
    assert len(audit.to_status("open")) == 1

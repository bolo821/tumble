from quill.audit.log import AuditLog


def test_record_and_query():
    log = AuditLog()
    log.record(at=1, actor="alice", kind="create", target="r1")
    log.record(at=2, actor="bob", kind="approve", target="r1")
    assert len(log) == 2
    assert len(log.for_actor("alice")) == 1
    assert len(log.for_target("r1")) == 2
    assert len(log.of_kind("create")) == 1

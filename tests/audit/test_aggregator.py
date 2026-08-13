from quill.audit.log import AuditLog
from quill.audit.aggregator import total_by_actor, total_by_kind, top_actors


def test_total_by_actor():
    log = AuditLog()
    log.record(at=1, actor="alice", kind="x", target="t")
    log.record(at=2, actor="alice", kind="x", target="t")
    log.record(at=3, actor="bob", kind="x", target="t")
    assert total_by_actor(log) == {"alice": 2, "bob": 1}


def test_top_actors():
    log = AuditLog()
    for _ in range(3):
        log.record(at=1, actor="alice", kind="x", target="t")
    log.record(at=1, actor="bob", kind="x", target="t")
    assert top_actors(log, n=1)[0] == ("alice", 3)

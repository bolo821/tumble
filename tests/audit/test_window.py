from quill.audit.log import AuditLog
from quill.audit.window import in_window, count_in_window


def test_in_window():
    log = AuditLog()
    log.record(at=5, actor="a", kind="x", target="t")
    log.record(at=15, actor="a", kind="x", target="t")
    log.record(at=25, actor="a", kind="x", target="t")
    assert len(in_window(log, start=10, end=20)) == 1


def test_count_in_window():
    log = AuditLog()
    log.record(at=10, actor="a", kind="x", target="t")
    log.record(at=20, actor="a", kind="x", target="t")
    assert count_in_window(log, start=0, end=30) == 2

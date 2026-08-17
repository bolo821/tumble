from quill.timeouts.tracker import TimeoutTracker


def test_start_and_expired():
    t = TimeoutTracker()
    t.start(check_id="c", at=0, timeout_seconds=10)
    assert not t.is_expired(check_id="c", now=5)
    assert t.is_expired(check_id="c", now=15)


def test_time_remaining():
    t = TimeoutTracker()
    t.start(check_id="c", at=0, timeout_seconds=10)
    assert t.time_remaining(check_id="c", now=3) == 7
    assert t.time_remaining(check_id="c", now=15) == 0


def test_expired_checks():
    t = TimeoutTracker()
    t.start(check_id="a", at=0, timeout_seconds=5)
    t.start(check_id="b", at=0, timeout_seconds=20)
    assert t.expired_checks(now=10) == ["a"]


def test_stop_removes():
    t = TimeoutTracker()
    t.start(check_id="c", at=0, timeout_seconds=10)
    t.stop("c")
    assert t.active_checks() == []

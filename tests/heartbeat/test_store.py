from quill.heartbeat.store import HeartbeatStore
from quill.heartbeat.registry import alive_count, dead_count, liveness_ratio


def test_record_and_is_alive():
    s = HeartbeatStore(timeout_seconds=10)
    s.record(key="ci", at=100)
    assert s.is_alive(key="ci", now=105)
    assert not s.is_alive(key="ci", now=200)


def test_alive_and_dead():
    s = HeartbeatStore(timeout_seconds=10)
    s.record(key="a", at=0)
    s.record(key="b", at=100)
    assert s.alive(now=101) == ["b"]
    assert s.dead(now=101) == ["a"]


def test_liveness_ratio():
    s = HeartbeatStore(timeout_seconds=10)
    s.record(key="a", at=0)
    s.record(key="b", at=100)
    assert liveness_ratio(s, now=101) == 0.5

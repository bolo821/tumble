from quill.timeouts.tracker import TimeoutTracker
from quill.timeouts.hard_kill import KillSink, kill_expired


def test_kill_expired():
    tracker = TimeoutTracker()
    tracker.start(check_id="a", at=0, timeout_seconds=5)
    sink = KillSink()
    kill_expired(tracker=tracker, sink=sink, now=10)
    assert sink.killed_ids() == ["a"]


def test_kill_sink_records():
    sink = KillSink()
    sink.kill(check_id="a", at=10)
    assert len(sink) == 1
    assert sink.for_check("a")[0]["at"] == 10

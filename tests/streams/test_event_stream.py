from quill.streams.event_stream import EventStream


def test_emit_then_for_review():
    s = EventStream()
    s.emit(at=10, kind="open", review_id="r")
    s.emit(at=20, kind="approve", review_id="r")
    assert len(s) == 2
    assert len(s.for_review("r")) == 2


def test_in_window():
    s = EventStream()
    s.emit(at=5, kind="x", review_id="r")
    s.emit(at=15, kind="x", review_id="r")
    assert len(s.in_window(start=10, end=20)) == 1


def test_of_kind():
    s = EventStream()
    s.emit(at=0, kind="open", review_id="r")
    s.emit(at=1, kind="approve", review_id="r")
    assert len(s.of_kind("open")) == 1

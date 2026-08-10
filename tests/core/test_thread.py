from quill.core.thread import Thread


def test_is_inline():
    t = Thread(id="t", review_id="r", file_path="a.py", line=1)
    assert t.is_inline()
    assert not t.is_general()


def test_is_general():
    t = Thread(id="t", review_id="r")
    assert t.is_general()
    assert not t.is_inline()

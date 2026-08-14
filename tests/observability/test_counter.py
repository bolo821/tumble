from quill.observability.counter import Counter


def test_inc_and_get():
    c = Counter()
    c.inc(key="a")
    c.inc(key="a")
    c.inc(key="b")
    assert c.get("a") == 2
    assert c.get("b") == 1
    assert c.total() == 3


def test_reset():
    c = Counter()
    c.inc(key="a")
    c.reset("a")
    assert c.get("a") == 0

from quill.observability.gauge import Gauge


def test_set_and_get():
    g = Gauge()
    g.set(key="a", value=5)
    assert g.get("a") == 5


def test_inc_dec():
    g = Gauge()
    g.inc(key="a", by=3)
    g.dec(key="a", by=1)
    assert g.get("a") == 2

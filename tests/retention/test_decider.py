from quill.core.review import Review
from quill.retention.policy import RetentionPolicy, PolicyRegistry
from quill.retention.decider import must_expire, time_to_expire


def _registry():
    reg = PolicyRegistry()
    reg.register(RetentionPolicy(label="tmp", keep_seconds=100))
    return reg


def test_must_expire_after_window():
    r = Review(id="r", title="t", author="a",
               labels=("tmp",), created_at=0)
    reg = _registry()
    assert not must_expire(review=r, registry=reg, now=99)
    assert must_expire(review=r, registry=reg, now=101)


def test_no_matching_label():
    r = Review(id="r", title="t", author="a",
               labels=("other",), created_at=0)
    assert not must_expire(review=r, registry=_registry(), now=1000)


def test_time_to_expire():
    r = Review(id="r", title="t", author="a",
               labels=("tmp",), created_at=0)
    assert time_to_expire(review=r, registry=_registry(), now=40) == 60

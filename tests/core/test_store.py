import pytest

from quill.core.review import Review
from quill.core.store import ReviewStore
from quill.errors import UnknownReview, DuplicateReview


def test_add_and_get():
    s = ReviewStore()
    r = Review(id="r", title="t", author="a")
    s.add(r)
    assert s.get("r") is r
    assert "r" in s


def test_duplicate_raises():
    s = ReviewStore()
    s.add(Review(id="r", title="t", author="a"))
    with pytest.raises(DuplicateReview):
        s.add(Review(id="r", title="other", author="a"))


def test_get_unknown_raises():
    s = ReviewStore()
    with pytest.raises(UnknownReview):
        s.get("missing")


def test_upsert_overwrites():
    s = ReviewStore()
    s.add(Review(id="r", title="t1", author="a"))
    s.upsert(Review(id="r", title="t2", author="a"))
    assert s.get("r").title == "t2"


def test_remove():
    s = ReviewStore()
    s.add(Review(id="r", title="t", author="a"))
    s.remove("r")
    assert "r" not in s

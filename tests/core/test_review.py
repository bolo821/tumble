from quill.core.review import Review, make_review
from quill.core.status import ReviewStatus


def test_make_review_defaults():
    r = make_review(id="r1", title="hi", author="alice")
    assert r.id == "r1"
    assert r.status == ReviewStatus.DRAFT
    assert r.base_branch == "main"


def test_label_set():
    r = Review(id="r", title="t", author="a", labels=("bug", "urgent"))
    assert r.label_set() == {"bug", "urgent"}
    assert r.has_label("bug")
    assert not r.has_label("nope")


def test_is_open():
    r = Review(id="r", title="t", author="a", status=ReviewStatus.OPEN)
    assert r.is_open()


def test_is_terminal():
    for s in (ReviewStatus.MERGED, ReviewStatus.CLOSED):
        r = Review(id="r", title="t", author="a", status=s)
        assert r.is_terminal()
    r_open = Review(id="r", title="t", author="a", status=ReviewStatus.OPEN)
    assert not r_open.is_terminal()

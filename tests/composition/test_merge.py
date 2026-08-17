import pytest

from quill.core.review import Review
from quill.core.store import ReviewStore
from quill.composition.merge import merge, merge_lenient, review_id_overlap
from quill.errors import DuplicateReview


def test_merge_disjoint():
    a = ReviewStore()
    a.add(Review(id="r1", title="t", author="a"))
    b = ReviewStore()
    b.add(Review(id="r2", title="t", author="a"))
    out = merge(a, b)
    assert len(out) == 2


def test_merge_overlapping_raises():
    a = ReviewStore()
    a.add(Review(id="r1", title="t", author="a"))
    b = ReviewStore()
    b.add(Review(id="r1", title="t", author="a"))
    with pytest.raises(DuplicateReview):
        merge(a, b)


def test_merge_lenient_overwrites():
    a = ReviewStore()
    a.add(Review(id="r", title="t1", author="a"))
    b = ReviewStore()
    b.add(Review(id="r", title="t2", author="a"))
    out = merge_lenient(a, b)
    assert out.get("r").title == "t2"


def test_overlap():
    a = ReviewStore()
    a.add(Review(id="r1", title="t", author="a"))
    a.add(Review(id="r2", title="t", author="a"))
    b = ReviewStore()
    b.add(Review(id="r2", title="t", author="a"))
    assert review_id_overlap(a, b) == ["r2"]

from quill.core.review import Review
from quill.core.store import ReviewStore
from quill.composition.diff import added, removed, changed


def test_added_removed():
    a = ReviewStore()
    a.add(Review(id="r1", title="t", author="alice"))
    b = ReviewStore()
    b.add(Review(id="r1", title="t", author="alice"))
    b.add(Review(id="r2", title="t", author="bob"))
    assert [r.id for r in added(a, b)] == ["r2"]
    assert removed(a, b) == []


def test_changed():
    a = ReviewStore()
    a.add(Review(id="r", title="t", author="a", status="open"))
    b = ReviewStore()
    b.add(Review(id="r", title="t", author="a", status="merged"))
    result = changed(a, b)
    assert len(result) == 1
    assert result[0][0].status == "open"
    assert result[0][1].status == "merged"

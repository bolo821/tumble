from quill.core.review import Review
from quill.labels.index import LabelIndex, reviews_with_all, reviews_with_any


def _index():
    idx = LabelIndex()
    idx.add(Review(id="r1", title="t", author="a", labels=("bug", "urgent")))
    idx.add(Review(id="r2", title="t", author="b", labels=("bug",)))
    idx.add(Review(id="r3", title="t", author="c", labels=("chore",)))
    return idx


def test_find():
    idx = _index()
    assert idx.find("bug") == ["r1", "r2"]


def test_count_for():
    idx = _index()
    assert idx.count_for("bug") == 2


def test_reviews_with_all():
    idx = _index()
    assert reviews_with_all(idx, ["bug", "urgent"]) == ["r1"]


def test_reviews_with_any():
    idx = _index()
    assert reviews_with_any(idx, ["urgent", "chore"]) == ["r1", "r3"]

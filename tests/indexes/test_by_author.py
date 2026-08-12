from quill.core.review import Review
from quill.indexes.by_author import ByAuthor


def test_add_and_find():
    idx = ByAuthor()
    idx.add(Review(id="r1", title="t", author="alice"))
    idx.add(Review(id="r2", title="t", author="alice"))
    idx.add(Review(id="r3", title="t", author="bob"))
    assert idx.find("alice") == ["r1", "r2"]
    assert idx.count_for("bob") == 1


def test_authors():
    idx = ByAuthor()
    idx.add(Review(id="r1", title="t", author="alice"))
    idx.add(Review(id="r2", title="t", author="bob"))
    assert idx.authors() == ["alice", "bob"]

from quill.indexes.by_reviewer import ByReviewer


def test_add_and_find():
    idx = ByReviewer()
    idx.add(review_id="r1", reviewer="alice")
    idx.add(review_id="r2", reviewer="alice")
    idx.add(review_id="r1", reviewer="bob")
    assert idx.find("alice") == ["r1", "r2"]
    assert idx.count_for("alice") == 2

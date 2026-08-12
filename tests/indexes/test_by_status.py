from quill.indexes.by_status import ByStatus


def test_set_moves_between_buckets():
    idx = ByStatus()
    idx.set(review_id="r", status="open")
    assert idx.find("open") == ["r"]
    idx.set(review_id="r", status="merged")
    assert idx.find("open") == []
    assert idx.find("merged") == ["r"]


def test_count_for():
    idx = ByStatus()
    idx.set(review_id="a", status="open")
    idx.set(review_id="b", status="open")
    assert idx.count_for("open") == 2

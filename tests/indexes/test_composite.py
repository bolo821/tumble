from quill.core.review import Review
from quill.indexes.by_author import ByAuthor
from quill.indexes.by_status import ByStatus
from quill.indexes.composite import reviews_in_status_by_author


def test_intersection():
    by_author = ByAuthor()
    by_status = ByStatus()
    by_author.add(Review(id="r1", title="t", author="alice"))
    by_author.add(Review(id="r2", title="t", author="alice"))
    by_status.set(review_id="r1", status="open")
    by_status.set(review_id="r2", status="merged")
    result = reviews_in_status_by_author(
        by_status, by_author, status="open", author="alice",
    )
    assert result == ["r1"]

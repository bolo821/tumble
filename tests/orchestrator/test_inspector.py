from quill.core.review import Review
from quill.core.status import ReviewStatus
from quill.core.store import ReviewStore
from quill.orchestrator.inspector import Inspector


def _store():
    s = ReviewStore()
    s.add(Review(id="r1", title="t", author="a", status=ReviewStatus.OPEN))
    s.add(Review(id="r2", title="t", author="a", status=ReviewStatus.MERGED))
    return s


def test_total_and_by_status():
    ins = Inspector(store=_store())
    assert ins.total_reviews() == 2
    assert ins.open_count() == 1
    assert ins.merged_count() == 1


def test_overview():
    ins = Inspector(store=_store())
    ov = ins.overview()
    assert ov["total"] == 2
    assert ov["open"] == 1
    assert ov["merged"] == 1

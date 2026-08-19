from quill.core.review import Review
from quill.core.store import ReviewStore
from quill.bulk.op import BulkOp
from quill.bulk.processor import BulkProcessor
from quill.bulk.executors import label_executor


def _store_with_reviews():
    s = ReviewStore()
    s.add(Review(id="r1", title="t", author="alice"))
    s.add(Review(id="r2", title="t", author="alice"))
    return s


def test_apply_all_success():
    store = _store_with_reviews()
    op = BulkOp(kind="tag", target_review_ids=("r1", "r2"),
                parameters=(("label", "urgent"),))
    p = BulkProcessor()
    p.apply(op=op, store=store, executor=label_executor)
    assert p.completed_count() == 2
    assert store.get("r1").has_label("urgent")


def test_missing_review_fails():
    store = _store_with_reviews()
    op = BulkOp(kind="tag", target_review_ids=("r1", "missing"),
                parameters=(("label", "x"),))
    p = BulkProcessor()
    p.apply(op=op, store=store, executor=label_executor)
    assert p.completed_count() == 1
    assert p.failed_count() == 1
    assert p.failures()[0]["reason"] == "missing"

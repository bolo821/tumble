from quill.core.review import Review
from quill.core.store import ReviewStore
from quill.snapshots.recorder import from_store


def test_from_store():
    store = ReviewStore()
    store.add(Review(id="r1", title="t", author="alice"))
    snap = from_store(store, at=10)
    assert snap.at == 10
    assert snap.review_count() == 1
    assert snap.review_payload[0]["id"] == "r1"

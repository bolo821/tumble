from quill.snapshots.snapshot import Snapshot
from quill.snapshots.store import SnapshotStore


def test_record_and_latest():
    store = SnapshotStore()
    s = Snapshot(at=10, review_payload=())
    store.record(s)
    assert store.latest() is s


def test_in_window():
    store = SnapshotStore()
    store.record(Snapshot(at=5, review_payload=()))
    store.record(Snapshot(at=15, review_payload=()))
    assert len(store.in_window(start=10, end=20)) == 1

from quill.snapshots.snapshot import Snapshot
from quill.snapshots.diff import added_ids, removed_ids, status_changed


def _snap(at, entries):
    return Snapshot(at=at, review_payload=tuple(entries))


def test_added_ids():
    a = _snap(0, [{"id": "r1", "status": "open", "author": "a"}])
    b = _snap(10, [
        {"id": "r1", "status": "open", "author": "a"},
        {"id": "r2", "status": "open", "author": "a"},
    ])
    assert added_ids(a, b) == ["r2"]


def test_removed_ids():
    a = _snap(0, [{"id": "r1", "status": "open", "author": "a"}])
    b = _snap(10, [])
    assert removed_ids(a, b) == ["r1"]


def test_status_changed():
    a = _snap(0, [{"id": "r1", "status": "open", "author": "a"}])
    b = _snap(10, [{"id": "r1", "status": "merged", "author": "a"}])
    out = status_changed(a, b)
    assert out == [{"id": "r1", "from": "open", "to": "merged"}]

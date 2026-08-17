from quill.checkpoints.checkpoint import Checkpoint
from quill.checkpoints.store import CheckpointStore


def test_record_and_latest():
    s = CheckpointStore()
    c = Checkpoint(sequence=1, at=10, review_count=3)
    s.record(c)
    assert s.latest() is c


def test_all():
    s = CheckpointStore()
    s.record(Checkpoint(sequence=1, at=10, review_count=3))
    s.record(Checkpoint(sequence=2, at=20, review_count=5))
    assert len(s) == 2
    assert s.all()[-1].sequence == 2

from quill.checkpoints.checkpoint import Checkpoint
from quill.checkpoints.diff import review_delta, comment_delta, time_delta


def test_deltas():
    a = Checkpoint(sequence=1, at=10, review_count=3, comment_count=5)
    b = Checkpoint(sequence=2, at=20, review_count=5, comment_count=10)
    assert review_delta(a, b) == 2
    assert comment_delta(a, b) == 5
    assert time_delta(a, b) == 10


def test_none_returns_zero():
    assert review_delta(None, None) == 0

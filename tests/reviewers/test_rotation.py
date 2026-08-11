from quill.reviewers.rotation import RotationState


def test_next_cycles():
    r = RotationState()
    candidates = ["alice", "bob", "carol"]
    assert r.next(pool_key="k", candidates=candidates) == "alice"
    assert r.next(pool_key="k", candidates=candidates) == "bob"
    assert r.next(pool_key="k", candidates=candidates) == "carol"
    assert r.next(pool_key="k", candidates=candidates) == "alice"


def test_empty_candidates_returns_none():
    r = RotationState()
    assert r.next(pool_key="k", candidates=[]) is None

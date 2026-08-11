from quill.suggestions.suggestion import Suggestion
from quill.suggestions.apply import apply, discard, bulk_apply


def _sug():
    return Suggestion(id="s", comment_id="c", review_id="r",
                      file_path="x", original_lines=("a",),
                      proposed_lines=("b",))


def test_apply_marks_applied():
    s = _sug()
    assert apply(s, actor="alice", at=100)
    assert s.is_applied()
    assert s.applied_by == "alice"
    assert s.applied_at == 100


def test_apply_returns_false_if_not_pending():
    s = _sug()
    apply(s, actor="alice", at=100)
    assert not apply(s, actor="bob", at=200)


def test_discard():
    s = _sug()
    assert discard(s, actor="alice", at=50)
    assert s.is_discarded()


def test_bulk_apply():
    a = _sug()
    b = _sug()
    b = Suggestion(id="s2", comment_id="c", review_id="r",
                   file_path="x", original_lines=("a",),
                   proposed_lines=("b",))
    ids = bulk_apply([a, b], actor="alice", at=10)
    assert set(ids) == {"s", "s2"}

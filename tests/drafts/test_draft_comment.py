from quill.drafts.draft_comment import DraftComment, DraftStore


def test_stash_and_get():
    s = DraftStore()
    d = DraftComment(id="d1", review_id="r", author="alice",
                     body="hi", file_path="x.py", line=1)
    s.stash(d)
    assert s.get("d1") is d
    assert len(s.for_author("alice")) == 1
    assert d.is_inline()


def test_drop():
    s = DraftStore()
    s.stash(DraftComment(id="d", review_id="r", author="a", body="b"))
    s.drop("d")
    assert len(s) == 0

from quill.core.comment import Comment
from quill.comments.store import CommentStore


def test_add_and_query():
    s = CommentStore()
    s.add(Comment(id="c1", review_id="r1", thread_id="t1",
                  author="alice", body="hi"))
    s.add(Comment(id="c2", review_id="r1", thread_id="t2",
                  author="bob", body="hey", is_suggestion=True))
    assert s.get("c1").body == "hi"
    assert len(s.for_review("r1")) == 2
    assert len(s.for_thread("t1")) == 1
    assert len(s.by_author("alice")) == 1
    assert len(s.suggestions("r1")) == 1

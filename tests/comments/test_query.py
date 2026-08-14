from quill.core.comment import Comment
from quill.comments.store import CommentStore
from quill.comments.query import total_for_review, suggestion_count, authors_for_review


def test_total_for_review():
    s = CommentStore()
    s.add(Comment(id="c", review_id="r1", thread_id="t",
                  author="a", body="x"))
    s.add(Comment(id="c2", review_id="r1", thread_id="t",
                  author="b", body="y"))
    assert total_for_review(s, "r1") == 2


def test_authors_for_review():
    s = CommentStore()
    s.add(Comment(id="c", review_id="r", thread_id="t",
                  author="alice", body="x"))
    s.add(Comment(id="c2", review_id="r", thread_id="t",
                  author="bob", body="y"))
    assert authors_for_review(s, "r") == ["alice", "bob"]

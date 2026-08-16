from quill.drafts.draft_comment import DraftComment, DraftStore
from quill.comments.store import CommentStore
from quill.drafts.publish import publish_one, publish_all_for_author


def test_publish_one():
    draft = DraftComment(id="d", review_id="r", author="alice", body="hi")
    comment = publish_one(draft, comment_id="c", thread_id="t", at=10)
    assert comment.id == "c"
    assert comment.body == "hi"
    assert comment.created_at == 10


def test_publish_all_for_author():
    draft_store = DraftStore()
    draft_store.stash(DraftComment(id="d1", review_id="r", author="alice", body="a"))
    draft_store.stash(DraftComment(id="d2", review_id="r", author="alice", body="b"))
    comment_store = CommentStore()
    published = publish_all_for_author(
        draft_store, author="alice", comment_store=comment_store,
        thread_id="t", at=1, id_prefix="c",
    )
    assert len(published) == 2
    assert len(comment_store) == 2
    assert len(draft_store) == 0

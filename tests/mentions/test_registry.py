from quill.core.comment import Comment
from quill.mentions.registry import MentionRegistry


def test_index_and_query():
    reg = MentionRegistry()
    c = Comment(id="c1", review_id="r", thread_id="t",
                author="me", body="@alice @bob hey")
    reg.index(c)
    assert reg.for_comment("c1") == ["alice", "bob"]
    assert reg.comments_mentioning("alice") == ["c1"]
    assert reg.mention_count("alice") == 1


def test_all_mentioned():
    reg = MentionRegistry()
    c1 = Comment(id="c1", review_id="r", thread_id="t",
                 author="me", body="@alice")
    c2 = Comment(id="c2", review_id="r", thread_id="t",
                 author="me", body="@bob")
    reg.index(c1)
    reg.index(c2)
    assert reg.all_mentioned() == ["alice", "bob"]

from quill.reactions.reaction import Reaction, ReactionStore


def test_add_and_query():
    s = ReactionStore()
    s.add(Reaction(target_id="c1", reviewer_id="alice", emoji="+1", at=10))
    s.add(Reaction(target_id="c1", reviewer_id="bob", emoji="+1", at=20))
    assert len(s) == 2
    assert len(s.for_target("c1")) == 2
    assert len(s.with_emoji("+1")) == 2


def test_remove():
    s = ReactionStore()
    s.add(Reaction(target_id="c", reviewer_id="alice", emoji="+1", at=1))
    s.remove(target_id="c", reviewer_id="alice", emoji="+1")
    assert len(s) == 0

from quill.reactions.reaction import Reaction, ReactionStore
from quill.reactions.policy import has_already_reacted, can_react


def test_dedupe():
    s = ReactionStore()
    s.add(Reaction(target_id="c", reviewer_id="alice", emoji="+1", at=1))
    assert has_already_reacted(s, target_id="c",
                                reviewer_id="alice", emoji="+1")
    assert not has_already_reacted(s, target_id="c",
                                    reviewer_id="alice", emoji="heart")


def test_can_react_when_new():
    s = ReactionStore()
    ok, reason = can_react(s, target_id="c",
                           reviewer_id="alice", emoji="+1")
    assert ok

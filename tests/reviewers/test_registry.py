from quill.reviewers.reviewer import Reviewer, ReviewerRegistry


def test_register_and_get():
    reg = ReviewerRegistry()
    reg.register(Reviewer(id="alice"))
    assert reg.get("alice").id == "alice"


def test_with_group():
    reg = ReviewerRegistry()
    reg.register(Reviewer(id="alice", groups=("backend",)))
    reg.register(Reviewer(id="bob", groups=("frontend",)))
    assert len(reg.with_group("backend")) == 1
    assert reg.with_group("backend")[0].id == "alice"


def test_bots_vs_humans():
    reg = ReviewerRegistry()
    reg.register(Reviewer(id="ci", is_bot=True))
    reg.register(Reviewer(id="alice"))
    assert len(reg.bots()) == 1
    assert len(reg.humans()) == 1

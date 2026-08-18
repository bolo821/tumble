from quill.budgets.review_budget import ReviewBudget


def test_increment_and_remaining():
    b = ReviewBudget(cap=5)
    b.increment(reviewer="alice", by=2)
    assert b.active_count("alice") == 2
    assert b.remaining("alice") == 3


def test_is_exhausted():
    b = ReviewBudget(cap=3)
    b.increment(reviewer="alice", by=3)
    assert b.is_exhausted("alice")


def test_decrement():
    b = ReviewBudget(cap=5)
    b.increment(reviewer="alice", by=3)
    b.decrement(reviewer="alice", by=2)
    assert b.active_count("alice") == 1

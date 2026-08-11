from quill.reviewers.assignment import AssignmentStore


def test_assign_and_query():
    s = AssignmentStore()
    s.assign(review_id="r1", reviewer_id="alice")
    s.assign(review_id="r1", reviewer_id="bob")
    assert s.for_review("r1") == ["alice", "bob"]
    assert s.for_reviewer("alice") == ["r1"]
    assert s.is_assigned(review_id="r1", reviewer_id="alice")


def test_unassign():
    s = AssignmentStore()
    s.assign(review_id="r1", reviewer_id="alice")
    s.unassign(review_id="r1", reviewer_id="alice")
    assert not s.is_assigned(review_id="r1", reviewer_id="alice")


def test_total_assignments():
    s = AssignmentStore()
    s.assign(review_id="r1", reviewer_id="alice")
    s.assign(review_id="r2", reviewer_id="alice")
    assert s.total_assignments() == 2

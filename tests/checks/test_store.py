from quill.checks.check_run import CheckRun, SUCCESS, FAILURE, PENDING
from quill.checks.store import CheckRunStore


def test_add_and_query():
    s = CheckRunStore()
    s.add(CheckRun(id="c1", review_id="r", name="test", status=SUCCESS))
    s.add(CheckRun(id="c2", review_id="r", name="test", status=FAILURE))
    assert s.get("c1").status == SUCCESS
    assert len(s.for_review("r")) == 2
    assert len(s.by_name("r", name="test")) == 2


def test_latest_for():
    s = CheckRunStore()
    s.add(CheckRun(id="c1", review_id="r", name="test", status=SUCCESS))
    s.add(CheckRun(id="c2", review_id="r", name="test", status=FAILURE))
    latest = s.latest_for("r", name="test")
    assert latest.id == "c2"

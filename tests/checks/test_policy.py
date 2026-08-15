from quill.checks.check_run import CheckRun, SUCCESS, FAILURE
from quill.checks.store import CheckRunStore
from quill.checks.policy import is_all_green, failing_checks, missing_required


def test_all_green_when_success():
    s = CheckRunStore()
    s.add(CheckRun(id="c1", review_id="r", name="ci", status=SUCCESS))
    s.add(CheckRun(id="c2", review_id="r", name="lint", status=SUCCESS))
    assert is_all_green(store=s, review_id="r",
                        required_names=("ci", "lint"))


def test_not_all_green_when_failure():
    s = CheckRunStore()
    s.add(CheckRun(id="c1", review_id="r", name="ci", status=FAILURE))
    assert not is_all_green(store=s, review_id="r", required_names=("ci",))


def test_failing_checks():
    s = CheckRunStore()
    s.add(CheckRun(id="c1", review_id="r", name="ci", status=FAILURE))
    assert failing_checks(store=s, review_id="r") == ["ci"]


def test_missing_required():
    s = CheckRunStore()
    s.add(CheckRun(id="c1", review_id="r", name="ci", status=SUCCESS))
    assert missing_required(store=s, review_id="r",
                             required_names=("ci", "lint")) == ["lint"]

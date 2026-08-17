from quill.retries.policy import RetryPolicy, retries_remaining, should_retry


def test_max_attempts():
    p = RetryPolicy(max_attempts=3)
    assert should_retry(policy=p, attempt=1, signal="failure")
    assert not should_retry(policy=p, attempt=3, signal="failure")


def test_no_retry_on_signal():
    p = RetryPolicy(max_attempts=3, no_retry_on=("cancel",))
    assert not should_retry(policy=p, attempt=1, signal="cancel")


def test_retry_on_signal_whitelist():
    p = RetryPolicy(max_attempts=3, retry_on=("failure",))
    assert should_retry(policy=p, attempt=1, signal="failure")
    assert not should_retry(policy=p, attempt=1, signal="timeout")


def test_retries_remaining():
    p = RetryPolicy(max_attempts=5)
    assert retries_remaining(policy=p, attempt=2) == 3

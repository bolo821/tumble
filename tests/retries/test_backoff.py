from quill.retries.backoff import linear, exponential, fixed, jittered


def test_linear():
    assert linear(attempt=3, step_seconds=2) == 6


def test_exponential():
    assert exponential(attempt=1, base_seconds=2) == 2
    assert exponential(attempt=3, base_seconds=2) == 8


def test_exponential_capped():
    assert exponential(attempt=10, base_seconds=2, cap_seconds=50) == 50


def test_fixed():
    assert fixed(seconds=5) == 5


def test_jittered_deterministic():
    assert jittered(attempt=2, base_seconds=10, fraction=0.5) == 20

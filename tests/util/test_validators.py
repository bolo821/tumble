import pytest
from quill.util.validators import is_non_empty, in_range, is_one_of, all_unique, require


def test_is_non_empty():
    assert is_non_empty("x")
    assert not is_non_empty("   ")


def test_in_range():
    assert in_range(5, lo=0, hi=10)
    assert not in_range(11, lo=0, hi=10)


def test_is_one_of():
    assert is_one_of("a", ["a", "b"])


def test_all_unique():
    assert all_unique([1, 2, 3])
    assert not all_unique([1, 1])


def test_require_raises():
    with pytest.raises(ValueError):
        require(False, "nope")

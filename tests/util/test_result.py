import pytest
from quill.util.result import Ok, Err, all_ok, first_err, collect


def test_ok():
    r = Ok(5)
    assert r.is_ok()
    assert r.unwrap() == 5
    assert r.unwrap_or(0) == 5


def test_err():
    r = Err("nope")
    assert r.is_err()
    with pytest.raises(ValueError):
        r.unwrap()
    assert r.unwrap_or(3) == 3


def test_all_ok():
    assert all_ok([Ok(1), Ok(2)])
    assert not all_ok([Ok(1), Err("x")])


def test_first_err():
    assert first_err([Ok(1), Err("x")]).reason == "x"
    assert first_err([Ok(1), Ok(2)]) is None


def test_collect():
    r = collect([Ok(1), Ok(2)])
    assert r.is_ok()
    assert r.unwrap() == [1, 2]
    r2 = collect([Ok(1), Err("x")])
    assert r2.is_err()

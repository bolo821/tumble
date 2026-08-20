from quill.util.dicts import select_keys, without_keys, keys_with_value, merge_shallow, invert


def test_select_keys():
    assert select_keys({"a": 1, "b": 2}, "a") == {"a": 1}


def test_without_keys():
    assert without_keys({"a": 1, "b": 2}, "a") == {"b": 2}


def test_keys_with_value():
    assert keys_with_value({"a": 1, "b": 1, "c": 2}, 1) == ["a", "b"]


def test_merge_shallow():
    assert merge_shallow({"a": 1}, {"b": 2}) == {"a": 1, "b": 2}


def test_invert():
    assert invert({"a": 1, "b": 2}) == {1: "a", 2: "b"}

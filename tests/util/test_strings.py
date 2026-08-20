from quill.util.strings import truncate, pad_left, pad_right, safe_split, is_blank, common_prefix


def test_truncate_short_unchanged():
    assert truncate("hi", max_len=10) == "hi"


def test_truncate_long_ellipsis():
    assert truncate("a" * 10, max_len=6) == "aaa..."


def test_pad():
    assert pad_left("a", width=3) == "  a"
    assert pad_right("a", width=3) == "a  "


def test_safe_split():
    assert safe_split("a,b,c,d", ",", max_parts=2) == ["a", "b,c,d"]


def test_is_blank():
    assert is_blank("   ")
    assert not is_blank("x")


def test_common_prefix():
    assert common_prefix(["cat", "cattle"]) == "cat"
    assert common_prefix([]) == ""

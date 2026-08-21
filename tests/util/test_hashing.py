from quill.util.hashing import digest_hex, short_digest, combined_digest


def test_digest_hex_stable():
    assert digest_hex("hello") == digest_hex("hello")
    assert digest_hex("a") != digest_hex("b")


def test_short_digest_length():
    assert len(short_digest("x", length=6)) == 6


def test_combined_digest():
    assert combined_digest(["a", "b"]) == combined_digest(["a", "b"])
    assert combined_digest(["a", "b"]) != combined_digest(["b", "a"])

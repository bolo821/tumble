from quill.mentions.parser import parse, has_mention, mention_count, strip_mentions


def test_parse_extracts_mentions():
    assert parse("hey @alice and @bob") == ["alice", "bob"]


def test_no_mentions():
    assert parse("hello world") == []


def test_dedup():
    assert parse("@a and @a") == ["a"]


def test_has_mention():
    assert has_mention("@alice look", principal_id="alice")
    assert not has_mention("hello", principal_id="alice")


def test_strip_mentions():
    assert "@alice" not in strip_mentions("hi @alice")

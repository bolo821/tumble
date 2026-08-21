from quill.util.text_bounds import within_word_count, word_count, line_count, para_count


def test_within_word_count():
    assert within_word_count("one two three", lo=2, hi=5)
    assert not within_word_count("one", lo=2, hi=5)


def test_word_count():
    assert word_count("hi there") == 2
    assert word_count("") == 0


def test_line_count():
    assert line_count("a\nb\nc") == 3


def test_para_count():
    assert para_count("a\n\nb\n\nc") == 3

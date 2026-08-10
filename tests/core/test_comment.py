from quill.core.comment import Comment


def test_is_inline():
    c = Comment(id="c", review_id="r", thread_id="t",
                author="a", body="body", file_path="src/x.py", line=10)
    assert c.is_inline()
    c2 = Comment(id="c", review_id="r", thread_id="t",
                 author="a", body="body")
    assert not c2.is_inline()


def test_short_body():
    long = "x" * 100
    c = Comment(id="c", review_id="r", thread_id="t",
                author="a", body=long)
    assert len(c.short_body()) == 80

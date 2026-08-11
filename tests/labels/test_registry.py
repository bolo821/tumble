from quill.labels.label import Label
from quill.labels.registry import LabelRegistry


def test_register_and_query():
    reg = LabelRegistry()
    reg.register(Label(name="bug", color="red"))
    reg.register(Label(name="chore", color="blue"))
    assert reg.get("bug").color == "red"
    assert reg.names() == ["bug", "chore"]


def test_with_color():
    reg = LabelRegistry()
    reg.register(Label(name="a", color="red"))
    reg.register(Label(name="b", color="red"))
    reg.register(Label(name="c", color="blue"))
    assert len(reg.with_color("red")) == 2

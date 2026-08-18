from quill.variables.expansion import expand, referenced_vars, is_fully_expanded


def test_expand_variables():
    assert expand("hello $NAME", {"NAME": "world"}) == "hello world"
    assert expand("path is ${PATH}", {"PATH": "/usr"}) == "path is /usr"


def test_missing_var_left_alone():
    assert expand("$X and $Y", {"X": "1"}) == "1 and $Y"


def test_referenced_vars():
    assert referenced_vars("$A and $B and $A") == ["A", "B"]


def test_is_fully_expanded():
    assert is_fully_expanded("$X", {"X": "1"})
    assert not is_fully_expanded("$X", {})

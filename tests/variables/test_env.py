from quill.variables.env import EnvLayer


def test_push_and_get():
    e = EnvLayer()
    e.push(scope="a", values={"X": "1"})
    e.push(scope="b", values={"X": "2", "Y": "3"})
    assert e.get("X") == "2"
    assert e.get("Y") == "3"


def test_pop():
    e = EnvLayer()
    e.push(scope="a", values={"X": "1"})
    e.push(scope="b", values={"X": "2"})
    e.pop()
    assert e.get("X") == "1"


def test_flatten():
    e = EnvLayer()
    e.push(scope="a", values={"X": "1"})
    e.push(scope="b", values={"Y": "2"})
    flat = e.flatten()
    assert flat == {"X": "1", "Y": "2"}

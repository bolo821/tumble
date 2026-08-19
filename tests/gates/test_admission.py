from quill.gates.gate_state import GateState
from quill.gates.admission import is_admitted, missing_gates, admit_reason


def test_admitted_when_all_open():
    state = GateState().open("a").open("b")
    assert is_admitted(state=state, required_gates=("a", "b"))


def test_missing_gates():
    state = GateState().open("a")
    assert missing_gates(state=state, required_gates=("a", "b")) == ["b"]


def test_admit_reason():
    state = GateState().open("a")
    assert admit_reason(state=state, required_gates=("a",)) == "admitted"
    reason = admit_reason(state=state, required_gates=("a", "b"))
    assert "missing" in reason

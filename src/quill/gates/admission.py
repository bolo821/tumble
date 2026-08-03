"""admission: should a review be admitted given the current gate state?"""
from __future__ import annotations


def is_admitted(*, state, required_gates):
    return all(state.is_open(g) for g in required_gates)


def missing_gates(*, state, required_gates):
    return [g for g in required_gates if not state.is_open(g)]


def admit_reason(*, state, required_gates):
    missing = missing_gates(state=state, required_gates=required_gates)
    if not missing:
        return "admitted"
    return "missing-gates:" + ",".join(missing)

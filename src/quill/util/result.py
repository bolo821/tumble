"""Result: Ok/Err wrapper."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class Ok:
    value: object

    def is_ok(self):
        return True

    def is_err(self):
        return False

    def unwrap(self):
        return self.value

    def unwrap_or(self, default):
        return self.value


@dataclass(frozen=True)
class Err:
    reason: str

    def is_ok(self):
        return False

    def is_err(self):
        return True

    def unwrap(self):
        raise ValueError(self.reason)

    def unwrap_or(self, default):
        return default


def first_err(results):
    for r in results:
        if r.is_err():
            return r
    return None


def all_ok(results):
    return all(r.is_ok() for r in results)


def collect(results):
    out = []
    for r in results:
        if r.is_err():
            return r
        out.append(r.value)
    return Ok(out)

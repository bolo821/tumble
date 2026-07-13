"""identity: identity / no-op helpers."""
from __future__ import annotations


def identity(x):
    return x


def constant(value):
    def fn(*_args, **_kwargs):
        return value
    return fn


def always_true(*_args, **_kwargs):
    return True


def always_false(*_args, **_kwargs):
    return False


def noop(*_args, **_kwargs):
    return None


def compose(*fns):
    def fn(value):
        for f in reversed(fns):
            value = f(value)
        return value
    return fn

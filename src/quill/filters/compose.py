"""compose: combinators for filters."""
from __future__ import annotations


def both(a, b):
    def pred(x):
        return a(x) and b(x)
    return pred


def either(a, b):
    def pred(x):
        return a(x) or b(x)
    return pred


def neg(a):
    def pred(x):
        return not a(x)
    return pred


def all_of(*preds):
    def pred(x):
        return all(p(x) for p in preds)
    return pred


def any_of(*preds):
    def pred(x):
        return any(p(x) for p in preds)
    return pred

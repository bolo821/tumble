"""evaluator: evaluate WhenClause against context."""
from __future__ import annotations


def evaluate(clause, context):
    if clause.is_always():
        return True
    if clause.is_never():
        return False
    expr = clause.expression.strip()
    if "==" in expr:
        left, right = expr.split("==", 1)
        return str(context.get(left.strip(), "")) == _strip_quotes(right.strip())
    if "!=" in expr:
        left, right = expr.split("!=", 1)
        return str(context.get(left.strip(), "")) != _strip_quotes(right.strip())
    if expr.startswith("not "):
        return not bool(context.get(expr[4:].strip(), False))
    return bool(context.get(expr, False))


def _strip_quotes(s):
    if len(s) >= 2 and s[0] == s[-1] and s[0] in ('"', "'"):
        return s[1:-1]
    return s

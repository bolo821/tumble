"""compose: combine WhenClauses."""
from __future__ import annotations

from quill.conditions.evaluator import evaluate


def and_clauses(clauses, context):
    return all(evaluate(c, context) for c in clauses)


def or_clauses(clauses, context):
    return any(evaluate(c, context) for c in clauses)


def not_clause(clause, context):
    return not evaluate(clause, context)


def evaluate_named(*, named_clauses, context):
    return {name: evaluate(c, context) for name, c in named_clauses.items()}

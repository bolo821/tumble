"""coercion: string-to-typed helpers."""
from __future__ import annotations


def to_bool(value):
    if isinstance(value, bool):
        return value
    if value is None:
        return False
    return str(value).strip().lower() in ("1", "true", "yes", "on")


def to_int(value, *, default=0):
    try:
        return int(value)
    except (TypeError, ValueError):
        return default


def to_list(value, *, sep=","):
    if value is None:
        return []
    if isinstance(value, (list, tuple)):
        return list(value)
    return [v.strip() for v in str(value).split(sep) if v.strip()]


def to_string(value):
    if value is None:
        return ""
    return str(value)

"""text_helpers: format helpers."""
from __future__ import annotations


def section(title, *, width=60, fill="="):
    return fill * width + "\n" + title.center(width) + "\n" + fill * width


def two_column(label, value, *, label_width=20):
    return label.ljust(label_width) + str(value)


def bullet(items, *, prefix=" * "):
    return "\n".join(prefix + str(i) for i in items)


def indent(text, *, prefix="  "):
    return "\n".join(prefix + line for line in text.splitlines())


def join_with_truncate(items, *, sep=", ", max_items=5, ellipsis="..."):
    items = list(items)
    if len(items) <= max_items:
        return sep.join(str(i) for i in items)
    return sep.join(str(i) for i in items[:max_items]) + sep + ellipsis

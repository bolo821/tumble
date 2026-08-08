"""directive: parse review-directives from comment bodies."""
from __future__ import annotations

import re

_DIRECTIVE_RE = re.compile(r"^/(\w+)(?:\s+(.*))?$", re.MULTILINE)


def parse(body):
    out = []
    for m in _DIRECTIVE_RE.finditer(body):
        out.append((m.group(1), (m.group(2) or "").strip()))
    return out


def has_directive(body, *, name):
    return any(d == name for d, _ in parse(body))


def argument_of(body, *, name):
    for d, arg in parse(body):
        if d == name:
            return arg
    return None


def directive_count(body):
    return len(parse(body))

"""expansion: substitute $VAR / ${VAR} in template strings."""
from __future__ import annotations

import re

_PAT = re.compile(r"\$\{?([A-Z_][A-Z0-9_]*)\}?")


def expand(template, env):
    def repl(match):
        key = match.group(1)
        if key in env:
            return str(env[key])
        return match.group(0)
    return _PAT.sub(repl, template)


def referenced_vars(template):
    return sorted(set(_PAT.findall(template)))


def is_fully_expanded(template, env):
    return all(v in env for v in referenced_vars(template))

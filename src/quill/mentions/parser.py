"""parser: extract @mentions from comment bodies."""
from __future__ import annotations

import re

_MENTION_RE = re.compile(r"@([a-zA-Z0-9][-a-zA-Z0-9_]{0,38})")


def parse(text):
    return sorted(set(_MENTION_RE.findall(text)))


def has_mention(text, *, principal_id):
    return principal_id in parse(text)


def mention_count(text):
    return len(parse(text))


def strip_mentions(text):
    return _MENTION_RE.sub("", text)

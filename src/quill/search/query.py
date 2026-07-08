"""Query: structured review search expression."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class Query:
    title_substring: str = ""
    authors: tuple = ()
    labels_all: tuple = ()
    labels_any: tuple = ()
    statuses: tuple = ()
    files: tuple = ()

    def is_empty(self):
        return (
            not self.title_substring
            and not self.authors
            and not self.labels_all
            and not self.labels_any
            and not self.statuses
            and not self.files
        )

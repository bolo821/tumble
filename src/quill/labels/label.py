"""Label: a labeled marker attached to reviews."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class Label:
    name: str
    color: str = ""
    description: str = ""

    def display(self):
        return self.name


def make_label(name, *, color="", description=""):
    return Label(name=name, color=color, description=description)

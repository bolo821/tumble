"""text_bounds: check text bounds without dedicated libs."""
from __future__ import annotations


def within_word_count(text, *, lo, hi):
    words = text.split()
    return lo <= len(words) <= hi


def within_char_count(text, *, lo, hi):
    return lo <= len(text) <= hi


def word_count(text):
    return len(text.split())


def line_count(text):
    return len(text.splitlines())


def para_count(text):
    return sum(1 for chunk in text.split("\n\n") if chunk.strip())

"""formatter: format a Transcript for display."""
from __future__ import annotations


LEVEL_PADDING = {"info": "INFO ", "warn": "WARN ", "error": "ERROR"}


def format_line(line):
    level = LEVEL_PADDING.get(line["level"], line["level"].upper())
    return "[" + str(line["at"]).rjust(8) + "] " + level + " " + line["message"]


def format_lines(transcript):
    return [format_line(l) for l in transcript.lines()]


def format_section(transcript, *, level):
    return [format_line(l) for l in transcript.at_level(level)]


def format_window(transcript, *, start, end):
    return [format_line(l) for l in transcript.in_window(start=start, end=end)]

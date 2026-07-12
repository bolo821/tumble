"""time_helpers: pure integer-second helpers."""
from __future__ import annotations


SECONDS_PER_MINUTE = 60
SECONDS_PER_HOUR = 3600
SECONDS_PER_DAY = 86400


def minutes(n):
    return n * SECONDS_PER_MINUTE


def hours(n):
    return n * SECONDS_PER_HOUR


def days(n):
    return n * SECONDS_PER_DAY


def format_duration(seconds):
    if seconds < 0:
        seconds = 0
    if seconds < SECONDS_PER_MINUTE:
        return str(seconds) + "s"
    if seconds < SECONDS_PER_HOUR:
        return str(seconds // SECONDS_PER_MINUTE) + "m"
    if seconds < SECONDS_PER_DAY:
        return str(seconds // SECONDS_PER_HOUR) + "h"
    return str(seconds // SECONDS_PER_DAY) + "d"


def floor_to_minute(seconds):
    return (seconds // SECONDS_PER_MINUTE) * SECONDS_PER_MINUTE


def floor_to_hour(seconds):
    return (seconds // SECONDS_PER_HOUR) * SECONDS_PER_HOUR


def floor_to_day(seconds):
    return (seconds // SECONDS_PER_DAY) * SECONDS_PER_DAY

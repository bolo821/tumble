from quill.util.time_helpers import (
    minutes, hours, days, format_duration,
    floor_to_minute, floor_to_hour, floor_to_day,
)


def test_conversion():
    assert minutes(2) == 120
    assert hours(1) == 3600
    assert days(1) == 86400


def test_format_duration():
    assert format_duration(30) == "30s"
    assert format_duration(120) == "2m"
    assert format_duration(3600) == "1h"
    assert format_duration(86400) == "1d"


def test_floors():
    assert floor_to_minute(90) == 60
    assert floor_to_hour(7200) == 7200
    assert floor_to_day(90000) == 86400

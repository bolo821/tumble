from quill.core.review import Review
from quill.indexes.by_file import ByFile


def test_add_and_find():
    idx = ByFile()
    idx.add(Review(id="r1", title="t", author="a",
                   changed_files=("src/x.py", "src/y.py")))
    assert idx.find("src/x.py") == ["r1"]
    assert idx.count_for("src/y.py") == 1


def test_paths():
    idx = ByFile()
    idx.add(Review(id="r", title="t", author="a",
                   changed_files=("a.py", "b.py")))
    assert idx.paths() == ["a.py", "b.py"]

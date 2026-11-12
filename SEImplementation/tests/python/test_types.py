"""Test to optionally run mypy, when installed, on the Python sources"""

import os.path as pth

import sourcextractor

import pytest

try:
    import mypy.api
    HAVE_MYPY = True
except ImportError:
    HAVE_MYPY = False


@pytest.mark.skipif(not HAVE_MYPY, reason='mypy not installed')
def test_mypy(request) -> None:
    module_dirs = [pth.dirname(p) for p in sourcextractor.__path__]
    # Pick up the mypy.ini to use; the sourcextractor module is not a
    # namespace package so it should only have one path, and mypy.ini
    # is normally found in the first one
    assert module_dirs, 'sourcextractor module path not found'
    config_path = pth.join(module_dirs[0], 'mypy.ini')
    out, err, ret = mypy.api.run(['--config', config_path] + module_dirs)

    if ret != 0:
        dirs = ' '.join(f"'{d}'" for d in module_dirs)
        pytest.fail(f'{out}\n{err}run:\n\nmypy --config {config_path} {dirs}'
                    f'\n\nto run mypy directly and debug the issue')
    assert ret == 0, out + err

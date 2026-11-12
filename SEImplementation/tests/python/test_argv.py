"""
Parsing of --python-arg values by the config.argv helpers.

These are public API (star-exported from sourcextractor.config and published in
doc/src/config_api/argv.rst) but nothing in the tree used them until now.
"""

from __future__ import annotations

import sys

import pytest

from sourcextractor.config.argv import Arguments, FileList


@pytest.fixture
def no_args(monkeypatch: pytest.MonkeyPatch) -> None:
    """sourcextractor++ invoked with no --python-arg at all."""
    monkeypatch.setattr(sys, 'argv', ['config.py'])


@pytest.fixture
def python_args(monkeypatch: pytest.MonkeyPatch):
    """Set sys.argv to the given --python-arg values."""
    def set_args(*args: str) -> None:
        monkeypatch.setattr(sys, 'argv', ['config.py', *args])

    return set_args


class TestArguments:
    def test_value_spec_is_its_own_default(self, no_args) -> None:
        args = Arguments(zeropoint=25.7)
        assert args.zeropoint == 25.7

    def test_type_spec_defaults_to_none(self, no_args) -> None:
        args = Arguments(zeropoint=float)
        assert args.zeropoint is None

    def test_callable_spec_is_called_for_the_default(self, no_args) -> None:
        def parse(value: str = 'fallback') -> str:
            return value.upper()

        args = Arguments(label=parse)
        assert args.label == 'FALLBACK'

    def test_value_from_argv_is_cast_by_the_spec(self, python_args) -> None:
        python_args('zeropoint=22.5')
        args = Arguments(zeropoint=float)
        assert args.zeropoint == 22.5
        assert isinstance(args.zeropoint, float)

    def test_value_from_argv_overrides_the_default(self, python_args) -> None:
        python_args('zeropoint=22.5')
        assert Arguments(zeropoint=25.7).zeropoint == 22.5

    def test_only_the_first_equals_sign_splits(self, python_args) -> None:
        python_args('expr=a=b')
        assert Arguments(expr=str).expr == 'a=b'

    def test_unknown_parameter_raises_keyerror(self, python_args) -> None:
        python_args('zeropoint=22.5', 'nosuchthing=1')
        with pytest.raises(KeyError, match='Unknown parameters found: nosuchthing'):
            Arguments(zeropoint=float)

    def test_invalid_value_raises_valueerror(self, python_args) -> None:
        python_args('zeropoint=not-a-number')
        with pytest.raises(ValueError, match='Invalid value for zeropoint'):
            Arguments(zeropoint=float)

    def test_bare_flag_is_true(self, python_args) -> None:
        """A --python-arg with no '=' acts as a flag, and is recorded as True."""
        python_args('verbose')
        assert Arguments(verbose=False).verbose is True

    def test_undeclared_attribute_raises_attributeerror(self, no_args) -> None:
        args = Arguments(zeropoint=25.7)
        with pytest.raises(AttributeError):
            args.nosuchthing

    def test_getattr_on_uninitialized_instance(self) -> None:
        """
        Unlikely corner case but also a common nuisance when debugging classes
        that implement __getattr__.
        """
        args = object.__new__(Arguments)
        with pytest.raises(AttributeError):
            args.zeropoint


class TestArgumentsRoughEdges:
    """Current behaviour of the cases that are arguably wrong."""

    def test_bare_flag_is_cast_by_a_numeric_spec(self, python_args) -> None:
        """
        A flag is True rather than a string, so a spec that expects a value
        casts it instead of rejecting it: float(True) is 1.0.
        """
        python_args('zeropoint')
        assert Arguments(zeropoint=float).zeropoint == 1.0

    def test_bool_spec_cannot_express_false(self, python_args) -> None:
        """bool('false') is True, so declaring a bool type spec is a trap."""
        python_args('flag=false')
        assert Arguments(flag=bool).flag is True


class TestFileList:
    def test_sorted_alphabetically(self, tmp_path) -> None:
        for name in ('b.fits', 'a.fits', 'c.fits'):
            (tmp_path / name).touch()
        files = FileList(str(tmp_path / '*.fits'))
        assert [f.rsplit('/', 1)[-1] for f in files] == ['a.fits', 'b.fits', 'c.fits']
        assert len(files) == 3

    def test_no_matches_is_empty(self, tmp_path) -> None:
        files = FileList(str(tmp_path / '*.fits'))
        assert len(files) == 0
        assert list(files) == []

    def test_usable_as_an_arguments_spec(self, python_args, tmp_path) -> None:
        """FileList is documented as being passed straight to load_fits_images."""
        (tmp_path / 'a.fits').touch()
        python_args(f'images={tmp_path}/*.fits')
        args = Arguments(images=FileList)
        assert [f.rsplit('/', 1)[-1] for f in args.images] == ['a.fits']

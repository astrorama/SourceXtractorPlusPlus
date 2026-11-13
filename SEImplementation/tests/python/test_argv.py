"""
Parsing of --python-arg values by the config.argv helpers.

These are public API (star-exported from sourcextractor.config and published in
doc/src/config_api/argv.rst) but nothing in the tree used them until now.
"""

from __future__ import annotations

import sys
from collections.abc import Sequence
from typing import Literal

import pytest

from sourcextractor.config.argv import ArgumentError, Arguments, FileList


class BasicArgs(Arguments):
    hdu_num: int
    verbose: bool
    zeropoint: float = 1.0
    max_iterations: int = 1000


class TestArguments:
    """Tests of the new, declarative arguments interface"""

    def test_defaults(self) -> None:
        args = BasicArgs(['hdu_num=3'])
        assert args.hdu_num == 3
        assert args.verbose is False
        assert args.zeropoint == 1.0
        assert args.max_iterations == 1000

    def test_values_from_argv(self) -> None:
        args = BasicArgs(['hdu_num=3', 'verbose', 'zeropoint=22.5', 'max_iterations=10'])
        assert args.verbose is True
        assert args.zeropoint == 22.5
        assert args.max_iterations == 10

    def test_kebab_case_alias(self) -> None:
        args = BasicArgs(['hdu-num=3', 'max-iterations=10'])
        assert args.hdu_num == 3
        assert args.max_iterations == 10
        assert len(args.extra) == 0

        with pytest.raises(ArgumentError, match='given more than once'):
            BasicArgs(['hdu-num=3', 'hdu_num=4'])

    def test_required_argument(self) -> None:
        with pytest.raises(ArgumentError, match='hdu_num requires a value'):
            BasicArgs([])

    def test_flag_accepts_explicit_values(self) -> None:
        assert BasicArgs(['hdu_num=1', 'verbose=false']).verbose is False
        assert BasicArgs(['hdu_num=1', 'verbose=YES']).verbose is True

        with pytest.raises(ArgumentError, match='not a boolean'):
            BasicArgs(['hdu_num=1', 'verbose=perhaps'])

    def test_int_rejects_floats(self) -> None:
        """
        One of the rough edges of the legacy interface: an argument typed as
        `int` should not accept and silently truncate a float value
        """

        with pytest.raises(ArgumentError, match='Invalid value for hdu_num'):
            BasicArgs(['hdu_num=3.14'])

    def test_argument_given_as_flag_rejected(self) -> None:
        """
        One of the rough edges of the legacy interface: it would quietly set
        this to int(True) == 1, say
        """

        with pytest.raises(ArgumentError, match='requires a value'):
            BasicArgs(['hdu_num'])

    def test_unknown_errors_by_default(self) -> None:
        with pytest.raises(ArgumentError, match='Unknown arguments found: nope'):
            BasicArgs(['hdu-num=1', 'nope=1'])

    def test_reads_sys_argv_by_default(self, monkeypatch) -> None:
        monkeypatch.setattr(sys, 'argv', ['config.py', 'hdu-num=2'])
        assert BasicArgs(['hdu-num=2']).hdu_num == 2

    def test_takes_no_keyword_arguments(self) -> None:
        """
        The new interface deliberately does not accept keyword arguments, so
        as to differentiate from the old interface
        """

        with pytest.raises(TypeError, match='takes no keyword arguments'):
            BasicArgs(['hdu-num=1'], zeropoint=float)

    def test_extra_policies(self) -> None:
        class Warned(Arguments, extra='warn'):
            pass

        with pytest.warns(UserWarning, match='Unknown arguments found: foo'):
            args = Warned(['foo=bar'])

        # Unknown arguments are still allowed to be read under the warn policy
        assert 'foo' in args.extra
        assert args.foo == 'bar' == args.extra['foo']

        class Allowed(Arguments, extra='allow'):
            pass

        # No warnings, no nothing
        import warnings
        with warnings.catch_warnings():
            warnings.simplefilter('error')
            args2 = Allowed(['foo=bar'])

        assert 'foo' in args2.extra
        assert args2.foo == 'bar' == args2.extra['foo']

        def policy(name: str, value: object) -> Literal['allow', 'error']:
            """Custom policy allowing extra arguments beginning with 'assoc_'"""
            return 'allow' if name.startswith('assoc_') else 'error'

        class Custom(Arguments, extra=policy):
            pass

        args3 = Custom(['assoc_x=4.2'])
        assert args3.assoc_x == '4.2'

        with pytest.raises(ArgumentError, match='Unknown arguments found: foo'):
            Custom(['assoc_x=4.2', 'foo=bar'])

    def test_unknown_kebab_case_argument_normalized(self) -> None:
        class Allowed(Arguments, extra='allow'):
            pass

        assert Allowed(['thing-one=2']).thing_one == '2'


class MultiArgs(Arguments):
    ints: Sequence[int]
    strs: list[str] = []
    pair: tuple[float, float] = (0.0, 0.0)
    variadic: tuple[int, ...] = ()
    untyped: Sequence = ()
    heterogeneous: tuple[int, str] = (0, '')
    scalar: int = 0


class TestMultiValuedArguments:
    """
    Arguments declared with a sequence type may be given more than once

    Their value is always a sequence (typically a tuple, but may also be
    a list), even when the argument is given once.
    """

    def test_single_value_is_still_a_sequence(self) -> None:
        args = MultiArgs(['ints=1'])
        assert args.ints == (1,)

    def test_repeated_values(self) -> None:
        args = MultiArgs(['ints=1', 'ints=2', 'ints=3'])
        assert args.ints == (1, 2, 3)

    def test_items_are_converted_to_the_declared_type(self) -> None:
        args = MultiArgs(['ints=1', 'ints=2'])
        assert all(isinstance(i, int) for i in args.ints)

    def test_abstract_sequence_becomes_a_tuple(self) -> None:
        """Sequence[int] is abstract, so tuple is the default sequence type"""
        assert type(MultiArgs(['ints=1']).ints) is tuple

    def test_concrete_container_is_preserved(self) -> None:
        args = MultiArgs(['ints=1', 'strs=a', 'strs=b'])
        assert args.strs == ['a', 'b']
        assert type(args.strs) is list

    def test_unparameterized_sequence_defaults_to_str(self) -> None:
        args = MultiArgs(['ints=1', 'untyped=2', 'untyped=x'])
        assert args.untyped == ('2', 'x')

    def test_variadic_tuple_takes_any_number(self) -> None:
        assert MultiArgs(['ints=1', 'variadic=1']).variadic == (1,)
        assert MultiArgs(['ints=1', 'variadic=1', 'variadic=2']).variadic == (1, 2)

    def test_fixed_length_tuple_arity_is_checked(self) -> None:
        assert MultiArgs(['ints=1', 'pair=1', 'pair=2']).pair == (1.0, 2.0)

        with pytest.raises(ArgumentError, match='pair expects exactly 2 values, got 1'):
            MultiArgs(['ints=1', 'pair=1'])

        with pytest.raises(ArgumentError, match='pair expects exactly 2 values, got 3'):
            MultiArgs(['ints=1', 'pair=1', 'pair=2', 'pair=3'])

    def test_fixed_length_tuple_may_be_heterogeneous(self) -> None:
        args = MultiArgs(['ints=1', 'heterogeneous=5', 'heterogeneous=five'])
        assert args.heterogeneous == (5, 'five')

    def test_bad_item_value_is_rejected(self) -> None:
        with pytest.raises(ArgumentError,
                           match='Invalid value for ints: x .must be int.'):
            MultiArgs(['ints=x'])

    def test_flag_among_values_is_rejected(self) -> None:
        """A bare flag has no value, so it cannot be one of the items"""
        with pytest.raises(ArgumentError, match='ints requires a value'):
            MultiArgs(['ints=1', 'ints'])

    def test_scalar_argument_still_rejects_repeats(self) -> None:
        with pytest.raises(ArgumentError, match='does not support multiple values'):
            MultiArgs(['ints=1', 'scalar=1', 'scalar=2'])

    def test_str_is_not_treated_as_a_sequence(self) -> None:
        """
        str is technically a Sequence, but is wanted as a scalar

        Annoying Python wart when dealing with generic sequences...
        """
        class StrArg(Arguments):
            name: str = ''

        assert StrArg(['name=abc']).name == 'abc'
        with pytest.raises(ArgumentError, match='does not support multiple values'):
            StrArg(['name=a', 'name=b'])

    def test_repeats_mix_with_kebab_case(self) -> None:
        args = MultiArgs(['ints=1', 'ints=2'])
        assert args.ints == (1, 2)
        # the two spellings name the same argument, so they accumulate
        assert MultiArgs(['ints=1', 'ints=2', 'pair=1', 'pair=2']).pair == (1.0, 2.0)

    def test_required_sequence_must_be_given(self) -> None:
        with pytest.raises(ArgumentError, match='ints requires a value'):
            MultiArgs([])


class TestLegacyArguments:
    """Tests of the old Arguments interface"""

    def test_value_spec_is_its_own_default(self) -> None:
        args = Arguments([], zeropoint=25.7)
        assert args.zeropoint == 25.7

    def test_type_spec_defaults_to_none(self) -> None:
        args = Arguments([], zeropoint=float)
        assert args.zeropoint is None

    def test_callable_spec_is_called_for_the_default(self) -> None:
        def parse(value: str = 'fallback') -> str:
            return value.upper()

        args = Arguments([], label=parse)
        assert args.label == 'FALLBACK'

    def test_value_from_argv_is_cast_by_the_spec(self) -> None:
        args = Arguments(['zeropoint=22.5'], zeropoint=float)
        assert args.zeropoint == 22.5
        assert isinstance(args.zeropoint, float)

    def test_value_from_argv_overrides_the_default(self) -> None:
        args = Arguments(['zeropoint=22.5'], zeropoint=25.7)
        assert args.zeropoint == 22.5

    def test_only_the_first_equals_sign_splits(self) -> None:
        assert Arguments(['expr=a=b'], expr=str).expr == 'a=b'

    def test_unknown_parameter_raises_keyerror(self) -> None:
        with pytest.raises(KeyError, match='Unknown arguments found: nosuchthing'):
            Arguments(['zeropoint=22.5', 'nosuchthing=1'], zeropoint=float)

    def test_invalid_value_raises_valueerror(self) -> None:
        with pytest.raises(ValueError, match='Invalid value for zeropoint'):
            Arguments(['zeropoint=not-a-number'], zeropoint=float)

    def test_bare_flag_is_true(self) -> None:
        """A --python-arg with no '=' acts as a flag, and is recorded as True."""
        assert Arguments(['verbose'], verbose=False).verbose is True

    def test_undeclared_attribute_raises_attributeerror(self) -> None:
        args = Arguments([], zeropoint=25.7)
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


class TestLegacyArgumentsRoughEdges:
    """Current behaviour of the cases that are arguably wrong."""

    def test_bare_flag_is_cast_by_a_numeric_spec(self) -> None:
        """
        A flag is True rather than a string, so a spec that expects a value
        casts it instead of rejecting it: float(True) is 1.0.
        """
        assert Arguments(['zeropoint'], zeropoint=float).zeropoint == 1.0

    def test_bool_spec_cannot_express_false(self) -> None:
        """bool('false') is True, so declaring a bool type spec is a trap."""
        assert Arguments(['flag=false'], flag=bool).flag is True


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

    def test_usable_as_an_arguments_spec(self,  tmp_path) -> None:
        """FileList is documented as being passed straight to load_fits_images."""
        (tmp_path / 'a.fits').touch()
        args = Arguments([f'images={tmp_path}/*.fits'], images=FileList)
        assert [f.rsplit('/', 1)[-1] for f in args.images] == ['a.fits']

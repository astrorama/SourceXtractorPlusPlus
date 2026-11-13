# -*- coding: utf-8 -*-

# Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 3.0 of the License, or (at your option)
# any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
from __future__ import annotations

import inspect
import sys
import typing
import warnings
from collections import defaultdict
from collections.abc import Iterable, Iterator, Sequence
from dataclasses import dataclass
from enum import Enum
from glob import glob
from typing import Any, Callable, ClassVar, Literal, TypeAlias


_RawArgType: TypeAlias = str | bool | tuple[str | bool, ...]
_ExtraAction: TypeAlias = Literal['error', 'warn', 'allow']
_ExtraPolicy: TypeAlias = _ExtraAction | Callable[[str, _RawArgType], _ExtraAction]

_EXTRA_ACTIONS = ('error', 'warn', 'allow')
_TRUE = frozenset({'true', 'yes', 'on', '1'})
_FALSE = frozenset({'false', 'no', 'off', '0'})


class _Missing(Enum):
    MISSING = 0

    def __repr__(self) -> str:
        return '<required>'


MISSING = _Missing.MISSING


class ArgumentError(ValueError):
    """Error raised for a missing or malformed --python-arg"""


class Arguments:
    """
    This helper class automatically parses the arguments received by the Python script via
    sys.argv, using the set of key-value parameters received on its constructor to cast the types
    and define the defaults.

    Parameters
    ----------
    kwargs
        A set of key-values with the defaults, or the types of the expected values. For instance, it is valid to do
        both:

        key1 = 32.
        key2 = float

        For the first case - a value -, if no value is passed via sys.argv, this will be the default. Otherwise,
        its type (float) will be used to cast the received value.

        For the second case - a type -, if no value is passed via sys.argv, the parameter will default to None.
        Otherwise, the type will be used to cast the received value.

        Callables are also accepted. They should accept to be called with no parameters, returning the default,
        or with a single string, returning the parsed value.

        Values can be accessed later as attributes:

        instance.key1 and instance.key2

    Raises
    ------
    ValueError
        If the cast of the parameter failed (i.e. an invalid integer or float format)
    KeyError
        If additional unknown parameters are received
    """

    _declarative: ClassVar[bool] = False
    """Indicates that this is a subclass of Arguments using the new declarative interface"""

    _arg_specs: ClassVar[dict[str, _ArgSpec]] = {}
    _extra_policy: ClassVar[_ExtraPolicy] = 'error'
    """
    Policy for handled undeclared arguments

    Defaults to 'error' which was always the default behavior in the old non-declarative interface
    as well.
    """

    def __init_subclass__(cls, extra: _ExtraPolicy = 'error', **kwargs: Any) -> None:
        """
        Instantiate a subclass, processing its class-body annotations as a declarative argument
        specification
        """

        super().__init_subclass__(**kwargs)

        if not (callable(extra) or extra in _EXTRA_ACTIONS):
            raise ValueError(f'extra must be a callable or one of {_EXTRA_ACTIONS}, not {extra!r}')

        cls._declarative = True
        cls._extra_policy = extra
        cls._arg_specs = _arg_specs_from_annotations(cls)

    def __init__(self, argv: Iterable[str] | None = None, **kwargs: Any) -> None:
        """
        Constructor

        Parameters
        ----------
        argv : str iterable or None
            Arguments to parse, as normally passed from `--python-arg` arguments passed through
            `sys.argv`: this always defaults to ``sys.argv[1:]`` but may be overridden for testing.

        **kwargs : any
            Keyword arguments specifying the parsed argument names and their types/converters.
            This is only used for the old, non-declarative interface.
        """

        cls = type(self)
        if kwargs and cls._declarative:
            raise TypeError(f'{cls.__name__} declares its arguments in the class body; it takes '
                            f'no keyword arguments')

        if argv is None:
            argv = sys.argv[1:]

        if cls._declarative:
            arg_specs = cls._arg_specs
        else:
            arg_specs = {k: _ArgSpec.from_kwarg(k, v) for k, v in kwargs.items()}

        parsed_args = _parse_argv(argv)
        self.__args: dict[str, Any] = {}
        for name, spec in arg_specs.items():
            value = spec(parsed_args.pop(name, MISSING))
            self.__args[name] = value
            # shadow class-body defaults which would otherwise supersede __getattr__
            # during normal attribute lookups
            self.__dict__[name] = value

        # Only declare self.extra in the declarative interface; the old interface
        # never allowed unknown/extra arguments
        if cls._declarative:
            self.extra: dict[str, _RawArgType] = {}

        self.__collect_extra(parsed_args)

    def __collect_extra(self, parsed_args: dict[str, _RawArgType]) -> None:
        """Handle extra, undeclared arguments depending on the ``extra`` policy"""

        policy = type(self)._extra_policy
        unknown = []
        for name, raw in parsed_args.items():
            action = policy(name, raw) if callable(policy) else policy
            if action not in _EXTRA_ACTIONS:
                # This is already checked at class construction time, but in
                # the case of a per-arg callable we still need to check if
                # what it returned is a valid action
                raise ValueError(f'extra policy returned {action!r} for {name}; expected one of '
                                 f'{_EXTRA_ACTIONS}')

            if action == 'error' or action == 'warn':
                unknown.append(name)

            if action == 'error':
                continue

            self.extra[name] = raw
            self.__args[name] = raw

        if not unknown:
            return

        msg = f'Unknown arguments found: {", ".join(unknown)}'

        if not type(self)._declarative:
            # Fallback to KeyError: This is what the old, non-declarative
            # interface raised in this case so we keep that behavior for
            # backwards-compat
            raise KeyError(msg)
        elif action == 'warn':
            warnings.warn(msg, stacklevel=4)
            return
        elif action == 'allow':
            return

        raise ArgumentError(msg)

    def __getattr__(self, item: str) -> Any:
        """

        Parameters
        ----------
        item : str
            Name of the parameter to retrieve

        Returns
        -------
            The parsed value of the parameter

        Raises
        ------
        AttributeError
            If item has not been defined on construction
        """

        # Case for the arguments holder itself
        if item == '_Arguments__args':
            args: dict[str, Any] = {}
            self.__args = args
            return args

        if item not in self.__args:
            raise AttributeError(item)

        return self.__args[item]

    def __str__(self) -> str:
        """
        Returns
        -------
        str
            Human readable representation for the object
        """
        return str(self.__args)

    def __repr__(self) -> str:
        """
        Returns
        -------
        str
            String representation for the object
        """
        return repr(self.__args)


class FileList:
    """
    Helper class for receiving a globing pattern as a parameter, defining a list of files
    - i.e. measurement images or PSFs.
    It is an iterable, and can be passed directly to load_fits_images.

    Parameters
    ----------
    value : str
        A file globing expression. i.e "*.psf", "band_[r|i|g]_*.fits" or similar.
        The result is always stored and returned in alphabetical order, so the order between two
        file lists - i.e frame image and PSF - is consistent and a matching can be done easily between them.

    See Also
    --------
        glob.glob : Return a list of paths matching a pathname pattern.
    """

    def __init__(self, value: str) -> None:
        """
        Constructor.
        """
        self.__files = sorted(glob(value))

    def __iter__(self) -> Iterator[str]:
        """
        Returns
        -------
            iterator
        """
        return iter(self.__files)

    def __len__(self) -> int:
        """
        Returns
        -------
        int
            Length of the list
        """
        return len(self.__files)

    def __str__(self) -> str:
        """
        Returns
        -------
        str
            Human readable representation for the object
        """
        return str(self.__files)

    def __repr__(self) -> str:
        """
        Returns
        -------
        str
            String representation for the object
        """
        return repr(self.__files)


@dataclass(slots=True, frozen=True)
class _ArgSpec:
    """
    Specification for a declared argument

    Specs are built from `Arguments` subclass class-body annotations or from
    `Arguments.__init__` keyword arguments per the old.  The old interface
    maintains more permissive behavior w.r.t. argument types for
    backwards-compatibility, whereas the new interface gets ``strict=True``
    meaning, e.g., a value like ``3.14`` can't be passed to an argument typed
    `int`.
    """

    name: str
    type: Any
    default: Any
    strict: bool

    container: Any = None
    """Callable building the value of a multi-valued argument; None if scalar"""

    item_types: tuple[Any, ...] = ()
    """
    Type of each value of a multi-valued argument

    A single entry for a homogeneous sequence, or one per position for a
    fixed-length tuple such as ``tuple[float, float]``.
    """

    fixed: bool = False
    """Must the number of values match ``item_types`` exactly"""

    @classmethod
    def from_annotation(cls, name: str, hint: Any, default: Any) -> _ArgSpec:
        sequence = _sequence_hint(hint)
        if sequence is None:
            return cls(name, hint, default, strict=True)

        container, item_types, fixed = sequence
        return cls(name, hint, default, strict=True, container=container,
                   item_types=item_types, fixed=fixed)

    @classmethod
    def from_kwarg(cls, name: str, spec: Any) -> _ArgSpec:
        if isinstance(spec, type):
            return cls(name, spec, None, strict=False)
        elif callable(spec):
            return cls(name, spec, spec(), strict=False)
        return cls(name, type(spec), spec, strict=False)

    @property
    def is_flag(self) -> bool:
        """Is this argument a boolean flag"""
        return self.strict and self.type is bool

    @property
    def multi(self) -> bool:
        """May this argument be given more than once"""
        return self.container is not None

    @property
    def required(self) -> bool:
        """Is the argument required (has no default, and must be given)"""
        return self.strict and self.default is MISSING and not self.is_flag

    def __call__(self, raw: _RawArgType | _Missing) -> Any:
        """
        Convert a raw argument to the type declared by its spec

        The "raw" argument is typically a string: the ``v`` part of a ``k=v``
        argument/value pair passed via a ``--python-arg``.  It may also be
        a boolean (True) in the case of bool flag args.
        """

        if self.is_flag:
            if raw is True:
                return True
            elif raw is MISSING:
                return False

            assert isinstance(raw, str)
            if raw.lower() in _TRUE:
                return True
            if raw.lower() in _FALSE:
                return False
            raise ArgumentError(f'Invalid value for {self.name}: {raw!r} is not a boolean')

        if raw is MISSING:
            if self.default is not MISSING:
                return self.default

            raise ArgumentError(f'{self.name} requires a value ({self.name}=...)')

        if self.multi:
            return self.__convert_sequence(raw)

        if isinstance(raw, tuple):
            raise ArgumentError(f'{self.name} given more than once and does not support '
                                f'multiple values')

        return self.__convert_item(self.type, raw)

    def __convert_sequence(self, raw: _RawArgType) -> Any:
        """Convert the values of a multi-valued argument and build its container"""

        # A multi-valued argument given once is still a sequence, of one value
        raws = raw if isinstance(raw, tuple) else (raw,)

        if self.fixed and len(raws) != len(self.item_types):
            raise ArgumentError(f'{self.name} expects exactly {len(self.item_types)} '
                                f'values, got {len(raws)}')

        item_types = self.item_types if self.fixed else self.item_types * len(raws)
        return self.container(self.__convert_item(item_type, value)
                              for item_type, value in zip(item_types, raws))

    def __convert_item(self, item_type: Any, raw: str | bool) -> Any:
        """Convert a single raw value to ``item_type``"""

        if self.strict and raw is True:
            # a bare flag given for an argument that takes a value; the old
            # interface instead casts it, e.g. float(True) == 1.0
            raise ArgumentError(f'{self.name} requires a value ({self.name}=...)')

        try:
            return item_type(raw)
        except (TypeError, ValueError):
            raise ArgumentError(f'Invalid value for {self.name}: {raw} '
                                f'(must be {_type_name(item_type)})') from None


def _arg_specs_from_annotations(cls: type) -> dict[str, _ArgSpec]:
    """Convert class-body annotations to a collection of `_ArgSpec`"""

    specs: dict[str, _ArgSpec] = {}

    for name, hint in typing.get_type_hints(cls).items():
        if name.startswith('_') or typing.get_origin(hint) is ClassVar:
            # Ignore 'private' members and classvars
            continue

        if name == 'extra':
            raise ArgumentError(f"{cls.__name__}: the 'extra' attribute is reserved for the "
                                f"dict of undeclared arguments and cannot be used as an argument "
                                f"name")

        specs[name] = _ArgSpec.from_annotation(name, hint, getattr(cls, name, MISSING))

    return specs


def _type_name(type_: Any) -> str:
    """Readable name for a type or converter, which may be a generic alias"""

    return getattr(type_, '__name__', None) or str(type_)


def _sequence_hint(hint: Any) -> tuple[Any, tuple[Any, ...], bool] | None:
    """
    Interpret a type hint denoting a sequence of scalars

    Such an argument is multi-valued: it may be given more than once, and its
    value is always a sequence even when given once.  Returns None for a hint
    that is not a sequence, including `str` and `bytes`, which are sequences
    but are wanted as scalars here.  Nested sequences are not supported.

    Returns
    -------
    container : callable
        Builds the value from an iterable.  An abstract hint such as
        ``Sequence[int]`` cannot be instantiated, so it becomes a `tuple`.

    item_types : tuple
        A single entry for a homogeneous sequence, or one per position for a
        fixed-length tuple.  Defaults to `str` if the hint is unparameterized.

    fixed : bool
        Whether the number of values must match ``item_types`` exactly.
    """

    origin = typing.get_origin(hint)
    target = origin if origin is not None else hint

    if not isinstance(target, type) or not issubclass(target, Sequence):
        return None

    if issubclass(target, (str, bytes)):
        return None

    container = tuple if inspect.isabstract(target) else target
    args = typing.get_args(hint)

    if not args:
        return container, (str,), False

    # tuple[float, float] is fixed-length and may be heterogeneous, whereas
    # tuple[int, ...] is homogeneous and of any length
    if target is tuple and args[-1] is not Ellipsis:
        return container, args, True

    return container, (args[0],), False


def _normalize_arg(arg: str) -> str:
    """kebab-case and snake_case spellings are treated as the same argument"""

    return arg.replace('-', '_')


def _parse_argv(argv: Iterable[str]) -> dict[str, _RawArgType]:
    """
    Split k=v arguments or flags

    An argument my also be given multiple times in which case it is returned
    as a tuple.

    Those without '=' are treated as boolean flags always valued True
    """

    parsed: defaultdict[str, list[str | bool]] = defaultdict(list)
    for arg in argv:
        key: str
        val: str | bool

        if '=' in arg:
            key, val = arg.split('=', 1)
        else:
            key, val = arg, True

        key = _normalize_arg(key)
        parsed[key].append(val)

    return {k: v[0] if len(v) == 1 else tuple(v) for k, v in parsed.items()}

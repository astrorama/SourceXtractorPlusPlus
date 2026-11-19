"""
Type stubs for the Boost.Python extension module

For now, only what the configuration package actually uses is declared; add to
it as the bindings in SEImplementation/src/lib/PythonConfig/PythonModule.cpp
grow.
"""

from __future__ import annotations

import enum
from collections.abc import Iterator


class Flags(enum.IntEnum):
    NONE = ...
    BIASED = ...
    SATURATED = ...
    BOUNDARY = ...
    NEIGHBORS = ...
    OUTSIDE = ...
    PARTIAL_FIT = ...
    INSUFFICIENT_DATA = ...
    ERROR = ...


class Id:
    @property
    def id(self) -> int: ...


class Aperture(Id):
    def __init__(self, apertures: list[float]) -> None: ...
    @property
    def apertures(self) -> list[float]: ...


class _StringStringMapEntry:
    """
    One entry of a _StringStringMap, as yielded by Boost.Python's
    map_indexing_suite

    Note there is no data() declared, although the proxy has one: calling it
    raises TypeError("No Python class registered for C++ class std::string"),
    because map_indexing_suite needs the value type registered to convert it.
    Read the value by subscripting the map with key() instead, which is what
    FitsFile.get_headers in measurement_images.py does.
    """

    def key(self) -> str: ...


class _StringStringMap:
    """
    A std::map<std::string, std::string> as exposed by Boost.Python
    """

    def __getitem__(self, key: str) -> str: ...
    def __len__(self) -> int: ...
    def __iter__(self) -> Iterator[_StringStringMapEntry]: ...


class FitsFile:
    def __init__(self, filename: str) -> None: ...
    @property
    def filename(self) -> str: ...
    @property
    def image_hdus(self) -> list[int]: ...
    def get_headers(self, hdu: int) -> _StringStringMap: ...
    def get_dimensions(self, hdu: int) -> list[int]: ...


class MeasurementImage(Id):
    def __init__(self, file: str, psf_file: str, weight_file: str) -> None: ...
    @property
    def file(self) -> str: ...
    @property
    def psf_file(self) -> str: ...
    @property
    def weight_file(self) -> str: ...
    gain: float
    saturation: float
    flux_scale: float
    weight_type: str
    weight_absolute: bool
    weight_scaling: float
    has_weight_threshold: bool
    weight_threshold: float
    is_background_constant: bool
    constant_background_value: float
    image_hdu: int
    psf_hdu: int
    weight_hdu: int
    image_name: str
    psf_name: str
    weight_name: str
    wcs_name: str
    is_data_cube: bool
    image_layer: int
    weight_layer: int
    psf_renormalize: bool

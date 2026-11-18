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
"""
The extents of an image, and the order in which they are indexed.

Kept in its own module, because the C++ extension imports it lazily to convert
SourceXtractor::Dimensions.
"""

from __future__ import annotations

from typing import Any, Literal

AxisOrder = Literal['xy', 'yx']

AXIS_ORDERS: tuple[AxisOrder, ...] = ('xy', 'yx')


class Dimensions(tuple[int, ...]):
    """
    The extents of an image, as a tuple of either two or three elements.

    Indexing follows ``axis_order``, so the ``width``, ``height`` and ``depth``
    properties are the unambiguous way to ask for one extent.

    Parameters
    ----------
    width : int
        Extent of the fastest-varying axis, FITS NAXIS1.
    height : int
        Extent of the next axis, FITS NAXIS2.
    depth : int
        Number of layers of a data cube, or 0 if this is not one.
    axis_order : {'yx', 'xy'}
        Order in which the extents are indexed.  'yx' is the NumPy convention,
        slowest axis first, and the default.  'xy' is the FITS convention,
        fastest axis first, and is what FitsFile.get_dimensions() reports.
    """

    _width: int
    _height: int
    _depth: int
    _axis_order: AxisOrder

    def __new__(cls, width: int, height: int, depth: int = 0,
                axis_order: AxisOrder = 'yx') -> Dimensions:
        if axis_order not in AXIS_ORDERS:
            raise ValueError(f'axis_order must be one of {AXIS_ORDERS}, '
                             f'not {axis_order!r}')

        if axis_order == 'xy':
            extents = (width, height, depth) if depth else (width, height)
        else:
            extents = (depth, height, width) if depth else (height, width)

        self = super().__new__(cls, extents)
        self._width = width
        self._height = height
        self._depth = depth
        self._axis_order = axis_order
        return self

    @property
    def width(self) -> int:
        """Extent of the fastest-varying axis, FITS NAXIS1"""
        return self._width

    @property
    def height(self) -> int:
        """Extent of the next axis, FITS NAXIS2"""
        return self._height

    @property
    def depth(self) -> int:
        """Number of layers, or 0 if this is not a data cube"""
        return self._depth

    @property
    def axis_order(self) -> AxisOrder:
        """Order in which the extents are indexed, 'yx' or 'xy'"""
        return self._axis_order

    @property
    def is_cube(self) -> bool:
        return self._depth > 0

    def __eq__(self, other: Any) -> Any:
        if isinstance(other, Dimensions):
            return (tuple(self) == tuple(other)
                    and self._axis_order == other._axis_order)

        # Deferring to tuple's own comparison, so that comparing against a
        # plain tuple compares the extents in index order
        return NotImplemented

    def __ne__(self, other: Any) -> Any:
        result = self.__eq__(other)
        return result if result is NotImplemented else not result

    # Defining __eq__ would otherwise make this unhashable
    __hash__ = tuple.__hash__

    def __repr__(self) -> str:
        parts = [f'width={self._width}', f'height={self._height}']
        if self.is_cube:
            parts.append(f'depth={self._depth}')
        parts.append(f'axis_order={self._axis_order!r}')
        return f'{type(self).__name__}({", ".join(parts)})'

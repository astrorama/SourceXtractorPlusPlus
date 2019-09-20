/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/*
 * FitsImageSource.cpp
 *
 *  Created on: Mar 21, 2018
 *      Author: mschefer
 */

#include "SEFramework/Image/FitsImageSource.h"

namespace SExtractor {

template <>
int FitsImageSource<double>::getDataType() const { return TDOUBLE; }

template <>
int FitsImageSource<float>::getDataType() const { return TFLOAT; }

template <>
int FitsImageSource<unsigned int>::getDataType() const { return TUINT; }

template <>
int FitsImageSource<int>::getDataType() const { return TINT; }

//FIXME what if compiled on 32bit system?
template <>
int FitsImageSource<long>::getDataType() const { return TLONGLONG; }

template <>
int FitsImageSource<long long>::getDataType() const { return TLONGLONG; }

template <>
int FitsImageSource<double>::getImageType() const { return DOUBLE_IMG; }

template <>
int FitsImageSource<float>::getImageType() const { return FLOAT_IMG; }

template <>
int FitsImageSource<unsigned int>::getImageType() const { return LONG_IMG; }

template <>
int FitsImageSource<int>::getImageType() const { return LONG_IMG; }

//FIXME what if compiled on 32bit system?
template <>
int FitsImageSource<long>::getImageType() const { return LONGLONG_IMG; }

template <>
int FitsImageSource<long long>::getImageType() const { return LONGLONG_IMG; }

//FIXME add missing types

}




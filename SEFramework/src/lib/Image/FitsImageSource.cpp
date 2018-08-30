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

//FIXME add missing types

}




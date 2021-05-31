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

#ifndef SOURCEXTRACTORPLUSPLUS_REPLACEUNDEFIMAGE_H
#define SOURCEXTRACTORPLUSPLUS_REPLACEUNDEFIMAGE_H

#include "SEFramework/Image/VectorImage.h"

namespace SourceXtractor {

/**
 * Replace undefined (i.e. masked) values with the average of the closest defined pixel values
 * @tparam T
 *  Pixel type
 * @param original
 *  Original image
 * @param mask
 *  Value that masks invalid pixels
 */
template <typename T>
std::shared_ptr<VectorImage<T>> ReplaceUndef(const VectorImage<T>& original, T mask);

// Instantiations
extern std::shared_ptr<VectorImage<SeFloat>> ReplaceUndef(const VectorImage<SeFloat>&, SeFloat);

} // end of namespace SourceXtractor

#endif // SOURCEXTRACTORPLUSPLUS_REPLACEUNDEFIMAGE_H

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
 * VectorImageDataVsModelInputTraits.h
 *
 *  Created on: Sep 20, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_IMAGE_VECTORIMAGEDATAVSMODELINPUTTRAITS_H_
#define _SEIMPLEMENTATION_IMAGE_VECTORIMAGEDATAVSMODELINPUTTRAITS_H_

#include "ModelFitting/Engine/DataVsModelInputTraits.h"
#include "SEImplementation/Image/ImageInterfaceTraits.h"

namespace ModelFitting {

template <>
struct DataVsModelInputTraits<ImageInterfaceTypePtr> {

  using iterator = decltype(ImageInterfaceTypePtr()->begin());

  static iterator begin(ImageInterfaceTypePtr& input) {
    return input->begin();
  }

  static iterator end(ImageInterfaceTypePtr& input) {
    return input->end();
  }

  static size_t size(const ImageInterfaceTypePtr& input) {
    return input->getWidth() * input->getHeight();
  }

};

} // end of namespace ModelFitting




#endif /* _SEIMPLEMENTATION_IMAGE_VECTORIMAGEDATAVSMODELINPUTTRAITS_H_ */

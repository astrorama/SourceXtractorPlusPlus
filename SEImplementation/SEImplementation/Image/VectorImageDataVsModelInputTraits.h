/*
 * VectorImageDataVsModelInputTraits.h
 *
 *  Created on: Sep 20, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_IMAGE_VECTORIMAGEDATAVSMODELINPUTTRAITS_H_
#define _SEIMPLEMENTATION_IMAGE_VECTORIMAGEDATAVSMODELINPUTTRAITS_H_

#include "ModelFitting/Engine/DataVsModelInputTraits.h"

#include "SEFramework/Image/VectorImage.h"

#include "SEImplementation/Image/ImageInterfaceTraits.h"

namespace ModelFitting {

template <>
struct DataVsModelInputTraits<ImageInterfaceTypePtr> {

  using iterator = decltype(ImageInterfaceTypePtr()->getData().begin());

  static iterator begin(ImageInterfaceTypePtr& input) {
    return input->getData().begin();
  }

  static iterator end(ImageInterfaceTypePtr& input) {
    return input->getData().end();
  }

  static size_t size(const ImageInterfaceTypePtr& input) {
    return input->getData().size();
  }

};

} // end of namespace ModelFitting




#endif /* _SEIMPLEMENTATION_IMAGE_VECTORIMAGEDATAVSMODELINPUTTRAITS_H_ */

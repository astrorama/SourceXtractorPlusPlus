/*
 * ImagePsf.h
 *
 *  Created on: Sep 19, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_IMAGE_IMAGEPSF_H_
#define _SEIMPLEMENTATION_IMAGE_IMAGEPSF_H_

#include "ElementsKernel/Exception.h"

#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Image/MultiplyImage.h"
#include "ModelFitting/utils.h"

#include "SEFramework/Convolution/Convolution.h"


namespace SExtractor {

class ImagePsf: public DFTConvolution<SeFloat, PaddedImage<SeFloat, Reflect101Coordinates>> {
private:
  typedef DFTConvolution<SeFloat, PaddedImage<SeFloat, Reflect101Coordinates>> base_t;

public:

  ImagePsf(double pixel_scale, std::shared_ptr<const VectorImage<SeFloat>> image)
          : base_t{image}, m_pixel_scale{pixel_scale} {
    if (image->getWidth() != image->getHeight()) {
      throw Elements::Exception() << "PSF kernel must be square but was "
                                  << image->getWidth() << " x " << image->getHeight();
    }
    if (image->getWidth() % 2 == 0) {
      throw Elements::Exception() << "PSF kernel must have odd size, but got "
                                  << image->getWidth();
    }
  }

  virtual ~ImagePsf() = default;

  double getPixelScale() const {
    return m_pixel_scale;
  }

  std::size_t getSize() const {
    return getWidth();
  }

  std::shared_ptr<VectorImage<SExtractor::SeFloat>> getScaledKernel(SeFloat scale) const {
    return VectorImage<SeFloat>::create(*MultiplyImage<SExtractor::SeFloat>::create(getKernel(), scale));
  }

private:
  double m_pixel_scale;

};

} // end of ModelFitting


#endif /* _SEIMPLEMENTATION_IMAGE_IMAGEPSF_H_ */

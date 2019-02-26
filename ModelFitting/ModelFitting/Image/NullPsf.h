/*
 * NullPsf.h
 *
 *  Created on: 2019 M01 9
 *      Author: mschefer
 */

#ifndef _MODELFITTING_IMAGE_NULLPSF_H_
#define _MODELFITTING_IMAGE_NULLPSF_H_

#include "ModelFitting/Image/ImageTraits.h"

// NullPsf, dummy type that satisfies the Psf trait but does nothing

namespace ModelFitting {

template <typename ImageType>
class NullPsf {
public:
  NullPsf() : m_kernel(ImageTraits<ImageType>::factory(1, 1)) {}

  double getPixelScale() const {
    return 1.0;
  }

  std::size_t getSize() const {
    return 1;
  }

  ImageType getScaledKernel(double /*scale*/) const {
    return m_kernel;
  }

  void convolve(ImageType& /*image*/) const {
  }

private:
  ImageType m_kernel;

};

}

#endif /* _MODELFITTING_IMAGE_NULLPSF_H_ */

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

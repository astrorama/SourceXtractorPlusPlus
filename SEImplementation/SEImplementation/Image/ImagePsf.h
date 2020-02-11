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
 * ImagePsf.h
 *
 *  Created on: Sep 19, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_IMAGE_IMAGEPSF_H_
#define _SEIMPLEMENTATION_IMAGE_IMAGEPSF_H_

#include "ElementsKernel/Exception.h"
#include "AlexandriaKernel/memory_tools.h"
#include "ModelFitting/Image/PsfTraits.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Image/ProcessedImage.h"
#include "SEFramework/Convolution/Convolution.h"


namespace SourceXtractor {

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

  std::shared_ptr<VectorImage<SourceXtractor::SeFloat>> getScaledKernel(SeFloat scale) const {
    return VectorImage<SeFloat>::create(*MultiplyImage<SourceXtractor::SeFloat>::create(getKernel(), scale));
  }

private:
  double m_pixel_scale;

};

} // end of SourceXtractor

namespace ModelFitting {

/**
 * Specialization of PsfTraits, as DFTConvolution has the concept of context
 */
template<>
struct PsfTraits<SourceXtractor::ImagePsf> {
  using context_t = typename std::unique_ptr<SourceXtractor::ImagePsf::ConvolutionContext>;
  static constexpr bool has_context = true;
};

} // end of ModelFitting


#endif /* _SEIMPLEMENTATION_IMAGE_IMAGEPSF_H_ */

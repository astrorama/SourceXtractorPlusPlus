/** Copyright © 2019-2021 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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
 * DownSampledImagePsf.h
 *
 *  Created on: Nov 5, 2021
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_IMAGE_DOWNSAMPLEDIMAGEPSF_H_
#define _SEIMPLEMENTATION_IMAGE_DOWNSAMPLEDIMAGEPSF_H_

#include "ElementsKernel/Exception.h"
#include "AlexandriaKernel/memory_tools.h"
#include "ModelFitting/Image/PsfTraits.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Image/ProcessedImage.h"
#include "SEFramework/Convolution/Convolution.h"

#include "SEImplementation/Image/ImagePsf.h"

namespace SourceXtractor {

/*
 * Encapsulates ImagePsf to provide a downscaled version for performance
 */
class DownSampledImagePsf {
public:

  DownSampledImagePsf(double pixel_scale, std::shared_ptr<VectorImage<SeFloat>> image, double down_scaling=1.0);

  virtual ~DownSampledImagePsf() = default;

  double getPixelScale() const;
  std::size_t getSize() const;
  std::shared_ptr<VectorImage<SourceXtractor::SeFloat>> getScaledKernel(SeFloat scale) const;
  void convolve(std::shared_ptr<WriteableImage<float>> image) const;

  // For ConvolutionContext optimization
  std::unique_ptr<DFTConvolution<SeFloat>::ConvolutionContext> prepare(const std::shared_ptr<const Image<SeFloat>>& model_ptr) const;
  void convolve(std::shared_ptr<WriteableImage<float>> image, std::unique_ptr<DFTConvolution<SeFloat>::ConvolutionContext>& context) const;

private:
  double m_down_scaling;
  std::shared_ptr<ImagePsf> m_psf;
};

} // end of SourceXtractor

namespace ModelFitting {

/**
 * Specialization of PsfTraits, as DFTConvolution has the concept of context
 */
template<>
struct PsfTraits<SourceXtractor::DownSampledImagePsf> {
  using context_t = typename std::unique_ptr<SourceXtractor::ImagePsf::ConvolutionContext>;
  static constexpr bool has_context = true;
};

} // end of ModelFitting

#endif /* _SEIMPLEMENTATION_IMAGE_DOWNSAMPLEDIMAGEPSF_H_ */

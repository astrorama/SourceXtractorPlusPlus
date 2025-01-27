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
 * DownSampledImagePsf.cpp
 *
 *  Created on: Nov 9, 2021
 *      Author: mschefer
 */

#include <numeric>
#include <iostream>

#include "SEImplementation/Image/ImageInterfaceTraits.h"
#include "SEImplementation/Image/DownSampledImagePsf.h"

namespace SourceXtractor {

DownSampledImagePsf::DownSampledImagePsf(
    double pixel_scale, std::shared_ptr<VectorImage<SeFloat>> image, double down_scaling, bool normalize_psf)
    : m_down_scaling(down_scaling), m_normalize_psf(normalize_psf)  {
  if (image != nullptr) {
    using Traits = ::ModelFitting::ImageTraits<std::shared_ptr<VectorImage<SeFloat>>>;

    if (image->getWidth() != image->getHeight()) {
      throw Elements::Exception() << "PSF kernel must be square but was "
                                  << image->getWidth() << " x " << image->getHeight();
    }
    if (image->getWidth() % 2 == 0) {
      throw Elements::Exception() << "PSF kernel must have odd size, but got "
                                  << image->getWidth();
    }

    if (down_scaling != 1.0) {
      int new_size = image->getWidth() * down_scaling;
      new_size += (new_size % 2 == 0) ? 1 : 0;

      auto new_image = Traits::factory(new_size, new_size);
      Traits::addImageToImage(new_image, image, down_scaling, new_size / 2.0, new_size / 2.0);

      // renormalize psf
      if (m_normalize_psf) {
        auto psf_sum = std::accumulate(new_image->getData().begin(), new_image->getData().end(), 0.);
        for (auto& pixel : new_image->getData()) {
          pixel /= psf_sum;
        }
      } else {
        double area_factor = 1.0 / (down_scaling * down_scaling);
        for (auto& pixel : new_image->getData()) {
          pixel *= area_factor;
        }
      }

      m_psf = std::make_shared<ImagePsf>(pixel_scale / down_scaling, new_image);
    } else {
      m_psf = std::make_shared<ImagePsf>(pixel_scale, image);
    }
  } else {
    m_down_scaling = 1.0;
  }
}

double DownSampledImagePsf::getPixelScale() const {
  if (m_psf != nullptr) {
    return m_psf->getPixelScale();
  } else {
    return 1.0;
  }
}

std::size_t DownSampledImagePsf::getSize() const {
  if (m_psf != nullptr) {
    return m_psf->getWidth();
  } else {
    return 1;
  }
}

std::shared_ptr<VectorImage<SourceXtractor::SeFloat>> DownSampledImagePsf::getScaledKernel(SeFloat scale) const {
  if (m_psf != nullptr) {
    return m_psf->getScaledKernel(scale);
  } else {
    return nullptr;
  }
}

void DownSampledImagePsf::convolve(std::shared_ptr<WriteableImage<float>> image) const {
  if (m_psf != nullptr) {
    m_psf->convolve(image);
  }
}

std::unique_ptr<DFTConvolution<SeFloat>::ConvolutionContext> DownSampledImagePsf::prepare(
    const std::shared_ptr<const Image<SeFloat>>& model_ptr) const {
  if (m_psf != nullptr) {
    return m_psf->prepare(model_ptr);
  } else {
    return nullptr;
  }
}

void DownSampledImagePsf::convolve(std::shared_ptr<WriteableImage<float>> image,
    std::unique_ptr<DFTConvolution<SeFloat>::ConvolutionContext>& context) const{
  if (m_psf != nullptr) {
    m_psf->convolve(image, context);
  }
}


}


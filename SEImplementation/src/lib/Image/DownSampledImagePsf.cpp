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

#include "SEImplementation/Image/ImageInterfaceTraits.h"
#include "SEImplementation/Image/DownSampledImagePsf.h"

namespace SourceXtractor {

namespace {

void renormalize(std::shared_ptr<VectorImage<SeFloat>> image, double flux) {
  double acc = 0.0;

  for (int y=0; y<image->getHeight(); y++) {
    for (int x=0; x<image->getWidth(); x++) {
      acc += image->getValue(x, y);
    }
  }

  if (acc > 0.0) {
    double scale = flux / acc;
    for (int y=0; y<image->getHeight(); y++) {
      for (int x=0; x<image->getWidth(); x++) {
        image->setValue(x, y, image->getValue(x, y) * scale);
      }
    }
  }
}

}

DownSampledImagePsf::DownSampledImagePsf(
    double pixel_scale, std::shared_ptr<VectorImage<SeFloat>> image, double down_scaling)
    : m_pixel_scale(pixel_scale), m_down_scaling(down_scaling) {

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
    renormalize(new_image, 1.0);

    m_psf_mip.emplace_back(std::make_shared<ImagePsf>(pixel_scale, new_image));
  } else {
    m_psf_mip.emplace_back(std::make_shared<ImagePsf>(pixel_scale, image));
  }

  //generateMips(image);
}

double DownSampledImagePsf::getPixelScale(int mip) const {
  return m_psf_mip.at(mip)->getPixelScale();
}

std::size_t DownSampledImagePsf::getSize(int mip) const {
  return m_psf_mip.at(mip)->getWidth();
}

std::shared_ptr<VectorImage<SourceXtractor::SeFloat>> DownSampledImagePsf::getScaledKernel(SeFloat scale, int mip) const {
  return VectorImage<SeFloat>::create(*MultiplyImage<SourceXtractor::SeFloat>::create(m_psf_mip.at(mip)->getKernel(), scale));
}

void DownSampledImagePsf::convolve(std::shared_ptr<WriteableImage<float>> image, double down_scaling) const {
  m_psf_mip.at(0)->convolve(image);
}

void DownSampledImagePsf::generateMips(std::shared_ptr<const VectorImage<SeFloat>> image) {
  int mip_size = image->getWidth();

  auto prev_image = image;

  for (int mip = 3; ; mip += 3) {
    mip_size = (mip_size + 2) / 3;
    mip_size += (mip_size % 2 == 0) ? 1 : 0;

    if (mip_size <= 1) {
      break;
    }

    auto new_image = VectorImage<SeFloat>::create(mip_size, mip_size);
    for (int y = 0; y < mip_size; y++) {
      for (int x = 0; x < mip_size; x++) {
        int ox = x * 3;
        int oy = y * 3;

        double acc = 0.0;
        for (int dy = 0; dy < 3; dy++) {
          for (int dx = 0; dx < 3; dx++) {
            if (ox + dx >= 0 && oy + dy >= 0) {
              acc += prev_image->getValue(ox + dx, oy + dy);
            }
          }
        }

        new_image->setValue(x, y, acc);
      }
    }

    m_psf_mip.emplace_back(std::make_shared<ImagePsf>(m_pixel_scale / mip, image));
  }
}

}


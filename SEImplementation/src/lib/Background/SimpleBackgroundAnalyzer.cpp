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
 * Background.cpp
 *
 *  Created on: Oct 11, 2016
 *      Author: mschefer
 */

#include "SEImplementation/Background/SimpleBackgroundAnalyzer.h"
#include "SEImplementation/Background/SE2BackgroundUtils.h"

#include <memory>
#include <algorithm>

#include "ElementsKernel/Logging.h"
#include "SEFramework/Image/ConstantImage.h"
#include "SEFramework/Image/VectorImage.h"


namespace SourceXtractor {

BackgroundModel SimpleBackgroundAnalyzer::analyzeBackground(
    std::shared_ptr<DetectionImage> image,
    std::shared_ptr<WeightImage> /*variance_map*/, std::shared_ptr<Image<unsigned char>> /*mask*/,
    WeightImage::PixelType /*variance_threshold*/) const {

  // FIXME we use a VectorImage the same size as input which won't allow larger than memory images to be processed

  auto image_copy = VectorImage<DetectionImage::PixelType>::create(*image);
  std::sort(image_copy->getData().begin(), image_copy->getData().end());
  bck_model_logger.debug() << "Using the SimpleBackgroundLeverAnalyzer";

  auto background_level = image_copy->getData()[image_copy->getData().size()/2]; // the median
  auto background_level_map = ConstantImage<SeFloat>::create(image->getWidth(), image->getHeight(), background_level);

  auto subtracted_image = SubtractImage<SeFloat>::create(image, background_level_map);

  auto background_variance = getVariance(subtracted_image);
  auto background_variance_map = ConstantImage<SeFloat>::create(image->getWidth(), image->getHeight(), background_variance);
  bck_model_logger.debug() << "bg: " << background_level << " var: " << background_variance;

  return BackgroundModel(background_level_map, background_variance_map, 1.0);
}


SeFloat SimpleBackgroundAnalyzer::getVariance(std::shared_ptr<DetectionImage> image) {
  // Note: We compute the RMS by only taking into consideration pixels
  // below the background value.

  double variance = 0;
  int pixels = 0;
  for (int y=0; y < image->getHeight(); y++) {
    for (int x=0; x < image->getWidth(); x++) {
      auto value = image->getValue(x, y);
      if (value < 0) {
        variance += value * value;
        pixels++;
      }
    }
  }

  if (pixels > 0) {
    variance /= pixels;
  }

  return variance;
}

}


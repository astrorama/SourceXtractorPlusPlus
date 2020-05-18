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

#include <memory>
#include <algorithm>

#include <iostream>

#include <boost/lexical_cast.hpp>
#include <SEFramework/Image/ProcessingImageSource.h>
#include "ElementsKernel/Exception.h"       // for Elements Exception
#include "ElementsKernel/Logging.h"         // for Logging::LogMessageStream, etc
#include "SEFramework/Image/ConstantImage.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Image/BufferedImage.h"
#include "SEImplementation/Background/Utils.h"
#include "SEImplementation/Background/SE2/SE2BackgroundUtils.h"
#include "SEImplementation/Background/SE2/SE2BackgroundModeller.h"
#include "SEImplementation/Background/SE2/SE2BackgroundLevelAnalyzer.h"


namespace SourceXtractor {

SE2BackgroundLevelAnalyzer::SE2BackgroundLevelAnalyzer(const std::vector<int>& cell_size,
                                                       const std::vector<int>& smoothing_box,
                                                       const WeightImageConfig::WeightType weight_type)
  : m_weight_type(weight_type)
{
  assert(cell_size.size() > 0 && cell_size.size() <= 2);
  assert(smoothing_box.size() > 0 && smoothing_box.size() <= 2);
  m_cell_size[0] = cell_size.front();
  m_cell_size[1] = cell_size.back();
  assert(m_cell_size[0] > 0 && m_cell_size[1] > 0);
  m_smoothing_box[0] = smoothing_box.front();
  m_smoothing_box[1] = smoothing_box.back();
  assert(m_smoothing_box[0] >= 0 && m_smoothing_box[1] >= 0);
}

BackgroundModel SE2BackgroundLevelAnalyzer::analyzeBackground(
    std::shared_ptr<DetectionImage> image,
    std::shared_ptr<WeightImage> variance_map, std::shared_ptr<Image<unsigned char>> mask,
    WeightImage::PixelType variance_threshold) const {

  if (mask!=nullptr)
  {
    bck_model_logger.debug() << "\tMask image with size: (" << mask->getWidth() << "," << mask->getHeight() << ")";

    // make sure the dimensions are the same
    if (image->getWidth()!=mask->getWidth())
      throw Elements::Exception() << "X-dims do not match: image=" << image->getWidth() << " mask=" << mask->getWidth();
    if (image->getHeight()!=mask->getHeight())
      throw Elements::Exception() << "Y-dims do not match: image=" << image->getHeight() << " mask=" << mask->getHeight();
  }

  if (variance_map!=nullptr)
  {
    bck_model_logger.debug() << "\tVariance image with size: (" << variance_map->getWidth() << "," << variance_map->getHeight() << ")";
    // make sure the dimensions are the same
    if (image->getWidth()!=variance_map->getWidth())
      throw Elements::Exception() << "X-dims do not match: image=" << image->getWidth() << " variance=" << variance_map->getWidth();
    if (image->getHeight()!=variance_map->getHeight())
      throw Elements::Exception() << "Y-dims do not match: image=" << image->getHeight() << " variance=" << variance_map->getHeight();
  }

  // create the background model
  SeFloat bck_median;
  SeFloat var_median;
  auto bck_model = fromSE2Modeller(image, variance_map, mask, variance_threshold, bck_median, var_median);

  // give some feedback
  bck_model_logger.info() << "Background for image: " << image->getRepr() << " median: " << bck_median << " rms: " << sqrt(var_median) << "!";

  // return model
  return bck_model;
}

BackgroundModel SE2BackgroundLevelAnalyzer::fromSE2Modeller(std::shared_ptr<DetectionImage> image, std::shared_ptr<WeightImage> variance_map, std::shared_ptr<Image<unsigned char>> mask, WeightImage::PixelType variance_threshold, SeFloat &bck_median, SeFloat &var_median) const {
  std::shared_ptr<SE2BackgroundModeller> bck_modeller(new SE2BackgroundModeller(image, variance_map, mask, 0x0001));
  std::shared_ptr<TypedSplineModelWrapper<SeFloat>> splModelBckPtr;
  std::shared_ptr<TypedSplineModelWrapper<SeFloat>> splModelVarPtr;

  PIXTYPE sigFac=0.0;
  //PIXTYPE weightThreshold=0.0;

  size_t bckCellSize[2] = {size_t(m_cell_size[0]),size_t(m_cell_size[1])};
  size_t filterBoxSize[2] = {size_t(m_smoothing_box[0]),size_t(m_smoothing_box[1])};

  // create the background model and the rms model
  bck_modeller->createSE2Models(splModelBckPtr, splModelVarPtr, sigFac, bckCellSize, variance_threshold, filterBoxSize);

  // assign the median and variance value
  bck_median = splModelBckPtr->getMedian();
  var_median = splModelVarPtr->getMedian();

  auto medianVariance = splModelVarPtr->getMedian();
  bck_model_logger.debug() << "\tMedian background value: "<< splModelBckPtr->getMedian();
  bck_model_logger.debug() << "\tMedian variance value: " << medianVariance;
  bck_model_logger.debug() << "\tScaling value: "<< sigFac;

  // check for the weight type
  if (m_weight_type == WeightImageConfig::WeightType::WEIGHT_TYPE_NONE) {
    bck_model_logger.debug() << "\tConstant variance image at value: "<< splModelVarPtr->getMedian();
    // create a background model using the splines and the variance with a constant image from the median value
    return BackgroundModel(BufferedImage<SeFloat>::create(splModelBckPtr),
                           ConstantImage<SeFloat>::create(image->getWidth(), image->getHeight(), splModelVarPtr->getMedian()),
                           99999, std::sqrt(medianVariance));
  }
  else {
    bck_model_logger.debug() << "\tVariable background and variance.";
    // return the variable background model
    return BackgroundModel(
        BufferedImage<SeFloat>::create(splModelBckPtr),
        BufferedImage<SeFloat>::create(splModelVarPtr),
        sigFac, std::sqrt(medianVariance)
    );
  }
}

}

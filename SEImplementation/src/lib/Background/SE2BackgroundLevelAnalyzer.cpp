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
#include "SEImplementation/Background/SE2BackgroundUtils.h"
#include "SEImplementation/Background/SE2BackgroundModeller.h"
#include "SEImplementation/Background/SE2BackgroundLevelAnalyzer.h"

#include "SEImplementation/Background/SimpleBackgroundAnalyzer.h"


namespace SExtractor {

SE2BackgroundLevelAnalyzer::SE2BackgroundLevelAnalyzer(const std::string &cell_size,
                                                       const std::string &smoothing_box,
                                                       const WeightImageConfig::WeightType weight_type): m_weight_type(weight_type)
{
  // initialize the parameters
  m_cell_size = stringToIntVec(cell_size, std::string(","));
  m_smoothing_box=stringToIntVec(smoothing_box,  std::string(","));

  // double a single paramter if necessary
  if (m_cell_size.size()<1){
    throw Elements::Exception() << "Can not convert to 'int': '" << cell_size;
  }
  else if (m_cell_size.size()<2){
    m_cell_size.push_back(m_cell_size[0]);
  }
  if (m_cell_size.size()<1){
    throw Elements::Exception() << "Can not convert to 'int': '" << smoothing_box;
  }
  else if (m_smoothing_box.size()<2){
    m_smoothing_box.push_back(m_smoothing_box[0]);
  }
}

BackgroundModel SE2BackgroundLevelAnalyzer::analyzeBackground(
    std::shared_ptr<DetectionImage> image,
    std::shared_ptr<WeightImage> variance_map, std::shared_ptr<Image<unsigned char>> mask,
    WeightImage::PixelType variance_threshold) const {

  bck_model_logger.debug() << "Analyzing background for " << image->getRepr();

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
  auto bck_model = fromSE2Modeller(image, variance_map, mask, variance_threshold);

  // return model
  return bck_model;
}

BackgroundModel SE2BackgroundLevelAnalyzer::fromSE2Modeller(std::shared_ptr<DetectionImage> image, std::shared_ptr<WeightImage> variance_map, std::shared_ptr<Image<unsigned char>> mask, WeightImage::PixelType variance_threshold) const {
  std::shared_ptr<SE2BackgroundModeller> bck_modeller(new SE2BackgroundModeller(image, variance_map, mask, 0x0001));
  std::shared_ptr<TypedSplineModelWrapper<SeFloat>> splModelBckPtr;
  std::shared_ptr<TypedSplineModelWrapper<SeFloat>> splModelVarPtr;

  PIXTYPE sigFac=0.0;
  //PIXTYPE weightThreshold=0.0;

  size_t bckCellSize[2] = {size_t(m_cell_size[0]),size_t(m_cell_size[1])};
  size_t filterBoxSize[2] = {size_t(m_smoothing_box[0]),size_t(m_smoothing_box[1])};

  // create the background model and the rms model
  bck_modeller->createSE2Models(splModelBckPtr, splModelVarPtr, sigFac, bckCellSize, variance_threshold, filterBoxSize);

  bck_model_logger.debug() << "\tMedian background value: "<< splModelBckPtr->getMedian();
  bck_model_logger.debug() << "\tMedian variance value: "<< splModelVarPtr->getMedian();
  bck_model_logger.debug() << "\tScaling value: "<< sigFac;

  // check for the weight type
  if (m_weight_type == WeightImageConfig::WeightType::WEIGHT_TYPE_NONE) {
    bck_model_logger.debug() << "\tConstant variance image at value: "<< splModelVarPtr->getMedian();
    // create a background model using the splines and the variance with a constant image from the median value
    return BackgroundModel(BufferedImage<SeFloat>::create(splModelBckPtr),
                           ConstantImage<SeFloat>::create(image->getWidth(), image->getHeight(), splModelVarPtr->getMedian()),
                           99999);
  }
  else {
    bck_model_logger.debug() << "\tVariable background and variance.";
    // return the variable background model
    return BackgroundModel(
        BufferedImage<SeFloat>::create(splModelBckPtr),
        BufferedImage<SeFloat>::create(splModelVarPtr),
        sigFac
    );
  }
}

std::vector<int> SE2BackgroundLevelAnalyzer::stringToIntVec(const std::string inString, const std::string delimiters)
{
  std::vector<int> result;
  int anInt=0;
  std::size_t first;
  std::size_t last;

  // convert the input string to a vector of strings along the commas
  std::vector<std::string> stringVec=stringSplit(inString, delimiters);

  // go over all members
  for (size_t index=0; index<stringVec.size(); index++)
  {
    // prepare trimming
    first = stringVec[index].find_first_not_of(' ');
    last  = stringVec[index].find_last_not_of(' ');

    try
    {
      // try converting to int and append to result vector
      anInt = boost::lexical_cast<size_t>(stringVec[index].substr(first, last-first+1));
      result.push_back(anInt);
    }
    catch ( const boost::bad_lexical_cast &exc ) // conversion failed, exception thrown by lexical_cast and caught
    {
      throw Elements::Exception() << "Can not convert to 'int': '" << stringVec[index].substr(first, last-first+1) << "'";
    }
  }

  return result;
}

std::vector<std::string> SE2BackgroundLevelAnalyzer::stringSplit(const std::string inString, const std::string delimiters)
{
  std::vector<std::string> result;
  std::string trimString;
  size_t current;
  size_t next = -1;
  size_t first=0;
  size_t last=0;

  // trim blanks from both sides;
  // return the empty vector if there
  // are only blanks
  first = inString.find_first_not_of(' ');
  if (first == std::string::npos)
    return result;
  last  = inString.find_last_not_of(' ');
  trimString = inString.substr(first, last-first+1);

  do
  { // split along the delimiter
    // and add to the result vector
    current = next + 1;
    next = trimString.find_first_of( delimiters, current );
    result.push_back(trimString.substr( current, next - current ));
  }
  while (next != std::string::npos);

  return result;
}


}

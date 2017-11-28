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
#include "ElementsKernel/Exception.h"       // for Elements Exception
#include "ElementsKernel/Logging.h"         // for Logging::LogMessageStream, etc
#include "SEFramework/Image/ConstantImage.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Image/FitsWriter.h"
#include "SEImplementation/Background/SE2BackgroundUtils.h"
#include "SEImplementation/Background/SE2BackgroundModeller.h"
#include "SEImplementation/Background/SE2BackgroundLevelAnalyzer.h"

namespace SExtractor {

SE2BackgroundLevelAnalyzer::SE2BackgroundLevelAnalyzer(const std::string &cell_size, const std::string &smoothing_box)
{
  // initialize the parameters
  m_cell_size = stringToIntVec(cell_size, std::string(","));
  m_smoothing_box=stringToIntVec(smoothing_box,  std::string(","));

  // double a single paramter if necessary
  if (m_cell_size.size()<1){
    throw Elements::Exception() << "Can not convert to 'int': '" << cell_size << "'!";
  }
  else if (m_cell_size.size()<2){
    m_cell_size.push_back(m_cell_size[0]);
  }
  if (m_cell_size.size()<1){
    throw Elements::Exception() << "Can not convert to 'int': '" << smoothing_box << "'!";
  }
  else if (m_smoothing_box.size()<2){
    m_smoothing_box.push_back(m_smoothing_box[0]);
  }
}

std::shared_ptr<Image<SeFloat>> SE2BackgroundLevelAnalyzer::analyzeBackground(
    std::shared_ptr<DetectionImage> image,
    std::shared_ptr<WeightImage> variance_map, std::shared_ptr<Image<unsigned char>> mask) const {

  if (mask!=nullptr)
  {
    //std::string bbb("mask.fits");
    //FitsWriter::writeFile(*mask, bbb);
    //std::cout << "Mask: " << mask->getWidth() << "," << mask->getHeight() << std::endl;
    //se2BckLog.info() << "Mask: " << mask->getWidth() << "," << mask->getHeight() << std::endl;
    //se2BckLog.info() << "Mask image with size: (" << mask->getWidth() << "," << mask->getHeight() << ")!";
    bck_model_logger.info() << "Mask image with size: (" << mask->getWidth() << "," << mask->getHeight() << ")!";
  }
  if (variance_map!=nullptr)
  {
  //std::string bbb("variance.fits");
  //FitsWriter::writeFile(*variance_map, bbb);
  //std::cout << "Variance: " << variance_map->getWidth() << "," << variance_map->getHeight() << std::endl;
    //se2BckLog.info() << "Variance image with size: (" << mask->getWidth() << "," << mask->getHeight() << ")!";
    bck_model_logger.info() << "Variance image with size: (" << mask->getWidth() << "," << mask->getHeight() << ")!";
  }

  // create the background model
  auto bck_image = fromSE2Modeller(image, variance_map, mask);

  // return model
  return bck_image;
}

std::shared_ptr<Image<SeFloat>> SE2BackgroundLevelAnalyzer::fromSE2Modeller(std::shared_ptr<DetectionImage> image, std::shared_ptr<WeightImage> variance_map, std::shared_ptr<Image<unsigned char>> mask) const {
  std::shared_ptr<SE2BackgroundModeller> bck_modeller(new SE2BackgroundModeller(image, variance_map, mask, 0x0001));
  std::shared_ptr<TypedSplineModelWrapper<SeFloat>> splModelBckPtr;
  std::shared_ptr<TypedSplineModelWrapper<SeFloat>> splModelSigPtr;

  PIXTYPE sigFac=0.0;
  PIXTYPE weightThreshold=0.0;

  size_t bckCellSize[2] = {size_t(m_cell_size[0]),size_t(m_cell_size[1])};
  size_t filterBoxSize[2] = {size_t(m_smoothing_box[0]),size_t(m_smoothing_box[1])};

  // create the background model and the rms model
  bck_modeller->createSE2Models(splModelBckPtr, splModelSigPtr, sigFac, bckCellSize, weightThreshold, filterBoxSize);

  // return the background
  return splModelBckPtr;
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
      throw Elements::Exception() << "Can not convert to 'int': '" << stringVec[index].substr(first, last-first+1) << "'!";
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

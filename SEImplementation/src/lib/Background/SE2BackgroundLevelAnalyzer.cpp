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
#include "SEImplementation/Background/SE2BackgroundModeller.h"
#include "SEImplementation/Background/SE2BackgroundLevelAnalyzer.h"

namespace SExtractor {

static Elements::Logging se2BckLog = Elements::Logging::getLogger("SE2Background");

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
    se2BckLog.info() << "Mask image with size: (" << mask->getWidth() << "," << mask->getHeight() << ")!";
  }
  if (variance_map!=nullptr)
  {
  //std::string bbb("variance.fits");
  //FitsWriter::writeFile(*variance_map, bbb);
  //std::cout << "Variance: " << variance_map->getWidth() << "," << variance_map->getHeight() << std::endl;
  se2BckLog.info() << "Variance image with size: (" << mask->getWidth() << "," << mask->getHeight() << ")!";
  }


  auto bck_image = fromSE2Modeller(image, variance_map, mask);
  //auto bck_image = fromMedianValue(image);
  return bck_image;
}

std::shared_ptr<Image<SeFloat>> SE2BackgroundLevelAnalyzer::fromMedianValue(std::shared_ptr<DetectionImage> image) const {
  auto image_copy = VectorImage<DetectionImage::PixelType>::create(*image);
  std::sort(image_copy->getData().begin(), image_copy->getData().end());
  return ConstantImage<SeFloat>::create(image->getWidth(), image->getHeight(), image_copy->getData()[image_copy->getData().size()/2]);
}

std::shared_ptr<Image<SeFloat>> SE2BackgroundLevelAnalyzer::fromSE2Modeller(std::shared_ptr<DetectionImage> image, std::shared_ptr<WeightImage> variance_map, std::shared_ptr<Image<unsigned char>> mask) const {
  //SE2BackgroundModeller* bck_modeller=NULL;
  std::shared_ptr<SE2BackgroundModeller> bck_modeller(new SE2BackgroundModeller(image, variance_map, mask, 0x0001));
  TypedSplineModelWrapper<SeFloat> *bckSpline=NULL;
  TypedSplineModelWrapper<SeFloat> *sigmaSpline=NULL;
  PIXTYPE sigFac=0.0;
  PIXTYPE weightThreshold=0.0;
  //size_t bckCellSize[2] = {size_t(m_cell_size),size_t(m_cell_size)};
  //size_t filterBoxSize[2] = {size_t(m_smoothing_box),size_t(m_smoothing_box)};
  size_t bckCellSize[2] = {size_t(m_cell_size[0]),size_t(m_cell_size[1])};
  size_t filterBoxSize[2] = {size_t(m_smoothing_box[0]),size_t(m_smoothing_box[1])};
  PIXTYPE* back_line = new PIXTYPE[image->getWidth()];

  //bck_modeller = new SE2BackgroundModeller(image, NULL, NULL, 0x0001);
  bck_modeller->createSE2Models(&bckSpline, &sigmaSpline, sigFac, bckCellSize, weightThreshold, filterBoxSize);

  //boost::filesystem::path outBck {"back.fits"};
  //boost::filesystem::path outSig {"sigma.fits"};
  //bckSpline->gridToFits(outBck);
  //sigmaSpline->gridToFits(outSig);

  // create the empty background image
  std::shared_ptr<VectorImage<SeFloat>> bck_image = VectorImage<SeFloat>::create(image->getWidth(), image->getHeight());

  // fill the background image with values
  for(long yIndex=0; yIndex<image->getHeight(); yIndex++)
  {
    // compute a background line
    bckSpline->splineLine(back_line, yIndex, 0, image->getWidth());
    for (long xIndex=0; xIndex<image->getWidth(); xIndex++)
      // set the line values
      bck_image->setValue((int)xIndex, (int)yIndex, (SeFloat)back_line[xIndex]);
  }
  std::string bbb("bbb.fits");
  //FitsWriter::writeFile(*bck_image, bbb);
  FitsWriter::writeFile(*bckSpline, bbb);

  // release memory
  if (bckSpline)
  {
    delete bckSpline;
    bckSpline=NULL;
  }
  if (sigmaSpline)
  {
    delete sigmaSpline;
    sigmaSpline= NULL;
  }
  delete back_line;

  return bck_image;
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

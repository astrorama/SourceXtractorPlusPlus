/*
 * Background.cpp
 *
 *  Created on: Oct 11, 2016
 *      Author: mschefer
 */

#include <memory>
#include <algorithm>

#include <iostream>

#include "ElementsKernel/Exception.h"       // for Elements Exception
#include "ElementsKernel/Logging.h"         // for Logging::LogMessageStream, etc
#include "SEFramework/Image/ConstantImage.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Image/FitsWriter.h"
#include "SEImplementation/Background/SE2BackgroundModeller.h"
#include "SEImplementation/Background/SE2BackgroundLevelAnalyzer.h"

namespace SExtractor {

static Elements::Logging se2BckLog = Elements::Logging::getLogger("SE2Background");


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

void SE2BackgroundLevelAnalyzer::setParameters(int cell_size, int smoothing_box) {

  // make sure that the inut values are reasonable
  if (cell_size <2){
    throw Elements::Exception() << "Can not accept value <2 for --cellsize-value=" << cell_size << "!";
  }
  if (smoothing_box <0){
    throw Elements::Exception() << "Can not accept value <0 for --smoothing-box-value=" << smoothing_box << "!";
  }

  // set the values
  m_cell_size=cell_size;
  m_smoothing_box=smoothing_box;
}

std::shared_ptr<Image<SeFloat>> SE2BackgroundLevelAnalyzer::fromSE2Modeller(std::shared_ptr<DetectionImage> image, std::shared_ptr<WeightImage> variance_map, std::shared_ptr<Image<unsigned char>> mask) const {
  //SE2BackgroundModeller* bck_modeller=NULL;
  std::shared_ptr<SE2BackgroundModeller> bck_modeller(new SE2BackgroundModeller(image, variance_map, mask, 0x0001));
  SplineModel* bckSpline=NULL;
  SplineModel* sigmaSpline=NULL;
  PIXTYPE sigFac=0.0;
  PIXTYPE weightThreshold=0.0;
  size_t bckCellSize[2] = {size_t(m_cell_size),size_t(m_cell_size)};
  size_t filterBoxSize[2] = {size_t(m_smoothing_box),size_t(m_smoothing_box)};
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
  //std::string bbb("bbb.fits");
  //FitsWriter::writeFile(*bck_image, bbb);

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
}

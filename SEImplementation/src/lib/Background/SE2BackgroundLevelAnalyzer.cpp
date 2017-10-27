/*
 * Background.cpp
 *
 *  Created on: Oct 11, 2016
 *      Author: mschefer
 */

#include <memory>
#include <algorithm>

#include <iostream>

#include "SEFramework/Image/ConstantImage.h"
#include "SEFramework/Image/VectorImage.h"

#include "SEImplementation/Background/SE2BackgroundLevelAnalyzer.h"

#include "SEImplementation/Background/BackgroundDefine.h"
#include "SEImplementation/Background/BackgroundCell.h"

namespace SExtractor {

std::shared_ptr<Image<SeFloat>> SE2BackgroundLevelAnalyzer::analyzeBackground(
    std::shared_ptr<DetectionImage> image,
    std::shared_ptr<WeightImage> variance_map, std::shared_ptr<Image<unsigned char>> mask) const {

  BackgroundCell* oneCell=NULL;
  PIXTYPE* gridData=NULL;
  gridData = new PIXTYPE[100];

  auto image_copy = VectorImage<DetectionImage::PixelType>::create(*image);
  std::sort(image_copy->getData().begin(), image_copy->getData().end());

  //std::cout << "\n\n\nOutput from stdout...\\n\n\n" << std::endl;
  //std::cout << image->getWidth() << " <--> " << image->getHeight() << std::endl;
  std::cout << "Using cell size: " << m_cell_size << std::endl;
  std::cout << "Using smoothing box: " << m_smoothing_box << std::endl;
  for (auto i=0; i<10; i++){
    for (auto j=0; j<10; j++){
      //std::cout <<  image_copy->getValue(PixelCoordinate(i,j));
      gridData[i*10+j] = (PIXTYPE)image_copy->getValue(PixelCoordinate(i,j));
    }
  }
  //oneCell = new BackgroundCell(gridData, nElements, weightData, weightVarThreshold);
  oneCell = new BackgroundCell(gridData, 100);

  delete gridData;
  delete oneCell;


  return ConstantImage<SeFloat>::create(image->getWidth(), image->getHeight(), image_copy->getData()[image_copy->getData().size()/2]);
}

}


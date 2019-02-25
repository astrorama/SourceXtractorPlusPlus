/*
 * MoffatCheckImage.cpp
 *
 *  Created on: 2019 M02 5
 *      Author: mschefer
 */


#include "SEFramework/Source/SourceGroupInterface.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/CheckImages/CheckImages.h"

#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelFitting.h"
#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelFittingUtils.h"

#include "SEImplementation/CheckImages/MoffatCheckImage.h"

namespace SExtractor {

using namespace ModelFitting;

void MoffatCheckImage::handleMessage(const std::shared_ptr<SourceGroupInterface>& group) {
  if (m_check_image) {

    for (auto& source : *group) {
      auto& model = source.getProperty<MoffatModelFitting>();

      if (model.getIterations() == 0) {
        continue;
      }

      MoffatModelEvaluator evaluator(source);
      CheckImages::getInstance().lock();
      for (int y=0; y<m_check_image->getHeight(); y++) {
        for (int x=0; x<m_check_image->getWidth(); x++) {
          m_check_image->setValue(x, y, m_check_image->getValue(x, y) + evaluator.getValue(x-0.5, y-0.5));
        }
      }
      CheckImages::getInstance().unlock();
    }
  }
}

}




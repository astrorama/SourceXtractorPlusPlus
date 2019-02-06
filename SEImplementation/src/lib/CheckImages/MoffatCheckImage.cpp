/*
 * MoffatCheckImage.cpp
 *
 *  Created on: 2019 M02 5
 *      Author: mschefer
 */


#include "SEFramework/Source/SourceGroupInterface.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/CheckImages/CheckImages.h"

#include "ModelFitting/utils.h"
#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Models/ExtendedModel.h"
#include "ModelFitting/Models/FlattenedMoffatComponent.h"
#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelFitting.h"

#include "SEImplementation/CheckImages/MoffatCheckImage.h"

namespace SExtractor {

using namespace ModelFitting;

void MoffatCheckImage::handleMessage(const std::shared_ptr<SourceGroupInterface>& group) {
  if (m_check_image) {
    CheckImages::getInstance().lock();

    for (auto& source : *group) {
      auto& model = source.getProperty<MoffatModelFitting>();

      if (model.getIterations() == 0) {
        continue;
      }

      ManualParameter x(model.getX());
      ManualParameter y(model.getY());
      ManualParameter moffat_i0(model.getMoffatI0());
      ManualParameter moffat_index(model.getMoffatIndex());
      ManualParameter minkowski_exponent(model.getMinkowksiExponent());
      ManualParameter flat_top_offset(model.getTopOffset());
      double size = model.getSize();
      ManualParameter x_scale(model.getXScale());
      ManualParameter y_scale(model.getYScale());
      ManualParameter moffat_rotation(model.getMoffatRotation());

      // Moffat model
      std::vector<std::unique_ptr<ModelComponent>> component_list {};
      auto moff = make_unique<FlattenedMoffatComponent>(moffat_i0, moffat_index, minkowski_exponent, flat_top_offset);
      component_list.clear();
      component_list.emplace_back(std::move(moff));
      ExtendedModel extended_model(std::move(component_list), x_scale, y_scale, moffat_rotation, size, size, x, y);

      for (int y=0; y<m_check_image->getHeight(); y++) {
        for (int x=0; x<m_check_image->getWidth(); x++) {
          m_check_image->setValue(x, y, m_check_image->getValue(x, y) + extended_model.getValue(x,y));
        }
      }
    }
    CheckImages::getInstance().unlock();
  }
}

}




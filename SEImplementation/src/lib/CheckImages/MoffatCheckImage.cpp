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
 * MoffatCheckImage.cpp
 *
 *  Created on: 2019 M02 5
 *      Author: mschefer
 */


#include "SEFramework/Source/SourceGroupInterface.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/CheckImages/CheckImages.h"

#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelFitting.h"
#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelEvaluator.h"

#include "SEImplementation/CheckImages/MoffatCheckImage.h"

namespace SourceXtractor {

using namespace ModelFitting;

void MoffatCheckImage::handleMessage(const std::shared_ptr<SourceGroupInterface>& group) {
  auto check_image = CheckImages::getInstance().getMoffatImage();
  if (check_image != nullptr) {
    for (auto& source : *group) {
      auto& model = source.getProperty<MoffatModelEvaluator>();

      if (model.getIterations() == 0) {
        continue;
      }

      for (int y=0; y<check_image->getHeight(); y++) {
        for (int x=0; x<check_image->getWidth(); x++) {
          check_image->setValue(x, y, check_image->getValue(x, y) + model.getValue(x - 0.5, y - 0.5));
        }
      }
    }
  }
}

}




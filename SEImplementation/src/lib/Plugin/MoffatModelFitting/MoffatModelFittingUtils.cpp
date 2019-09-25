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
 * MoffatModelFittingUtils.cpp
 *
 *  Created on: 2019 M02 20
 *      Author: mschefer
 */


#include "ModelFitting/utils.h"
#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Models/ExtendedModel.h"
#include "ModelFitting/Models/FlattenedMoffatComponent.h"

#include "SEImplementation/Image/ImageInterfaceTraits.h"

#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelFitting.h"
#include "SEImplementation/Plugin/MoffatModelFitting/MoffatModelFittingUtils.h"

namespace SExtractor {

using namespace ModelFitting;

MoffatModelEvaluator::MoffatModelEvaluator(const SourceInterface& source) {
  auto& model = source.getProperty<MoffatModelFitting>();

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

  m_model = std::make_shared<ExtendedModel<ImageInterfaceTypePtr>>(
      std::move(component_list), x_scale, y_scale, moffat_rotation, size, size, x, y);
}


}

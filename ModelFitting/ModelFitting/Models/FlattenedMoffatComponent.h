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
 * FlattenedMoffatComponent.h
 *
 *  Created on: 2019 M01 7
 *      Author: mschefer
 */

#ifndef _MODELFITTING_MODELS_FLATTENEDMOFFATCOMPONENT_H_
#define _MODELFITTING_MODELS_FLATTENEDMOFFATCOMPONENT_H_

#include "ModelFitting/Parameters/BasicParameter.h"

#include "ModelFitting/Models/ModelComponent.h"

namespace ModelFitting {

class FlattenedMoffatComponent : public ModelComponent {

public:
  FlattenedMoffatComponent(std::shared_ptr<BasicParameter>  max_intensity, std::shared_ptr<BasicParameter> moffat_index,
      std::shared_ptr<BasicParameter> minkowski_distance_param, std::shared_ptr<BasicParameter> flat_top_offset);

  virtual ~FlattenedMoffatComponent() = default;

  double getValue(double x, double y) override;

  void updateRasterizationInfo(double scale, double r_max) override;
  std::vector<ModelSample> getSharpSampling() override;
  bool insideSharpRegion(double x, double y) override;

private:
  std::shared_ptr<BasicParameter> m_max_intensity;
  std::shared_ptr<BasicParameter> m_moffat_index;
  std::shared_ptr<BasicParameter> m_minkowski_distance_param;
  std::shared_ptr<BasicParameter> m_flat_top_offset;
};

}

#endif /* _MODELFITTING_MODELS_FLATTENEDMOFFATCOMPONENT_H_ */

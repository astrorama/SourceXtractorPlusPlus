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
#include "ModelFitting/Parameters/ReferenceUpdater.h"

#include "ModelFitting/Models/ModelComponent.h"

namespace ModelFitting {

class FlattenedMoffatComponent : public ModelComponent {

public:

  FlattenedMoffatComponent(BasicParameter& max_intensity, BasicParameter& moffat_index,
      BasicParameter& minkowski_distance_param, BasicParameter& flat_top_offset);

  virtual ~FlattenedMoffatComponent() = default;

  virtual double getValue(double x, double y) override;

  virtual void updateRasterizationInfo(double scale, double r_max) override;
  virtual std::vector<ModelSample> getSharpSampling() override;
  virtual bool insideSharpRegion(double x, double y) override;

private:

  double m_max_intensity;
  ReferenceUpdater m_max_intensity_updater;

  double m_moffat_index;
  ReferenceUpdater m_moffat_index_updater;

  double m_minkowski_distance_param;
  ReferenceUpdater m_minkowski_distance_param_updater;

  double m_flat_top_offset;
  ReferenceUpdater m_flat_top_offset_updater;
};

}

#endif /* _MODELFITTING_MODELS_FLATTENEDMOFFATCOMPONENT_H_ */

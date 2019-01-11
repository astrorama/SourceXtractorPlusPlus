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

/*
 * CompactModelBase.h
 *
 *  Created on: Aug 19, 2019
 *      Author: mschefer
 */

#ifndef _MODELFITTING_MODELS_COMPACTMODELBASE_H_
#define _MODELFITTING_MODELS_COMPACTMODELBASE_H_

#include "ModelFitting/Parameters/BasicParameter.h"
#include "ModelFitting/Models/PositionedModel.h"

#include "SEUtils/Mat22.h"

namespace ModelFitting {

using SExtractor::Mat22;

template <typename ImageType>
class CompactModelBase : public ExtendedModel<ImageType> {

public:
  CompactModelBase(BasicParameter& x_scale, BasicParameter& y_scale,
                BasicParameter& rotation, double width, double height,
                BasicParameter& x, BasicParameter& y, std::tuple<double, double, double, double> transform);

  virtual ~CompactModelBase() = default;

protected:
  Mat22 getCombinedTransform(double pixel_scale) const;

  // Jacobian transform
  Mat22 m_jacobian;
  Mat22 m_inv_jacobian;

private:
  double m_x_scale;
  ReferenceUpdater m_x_scale_updater;

  double m_y_scale;
  ReferenceUpdater m_y_scale_updater;

  double m_rotation;
  ReferenceUpdater m_rotation_updater;
};

}

#include "_impl/CompactModelBase.icpp"


#endif /* _MODELFITTING_MODELS_COMPACTMODELBASE_H_ */

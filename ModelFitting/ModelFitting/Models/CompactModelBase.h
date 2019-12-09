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
  CompactModelBase(std::shared_ptr<BasicParameter> x_scale, std::shared_ptr<BasicParameter> y_scale,
      std::shared_ptr<BasicParameter> rotation, double width, double height,
      std::shared_ptr<BasicParameter> x, std::shared_ptr<BasicParameter> y,
      std::tuple<double, double, double, double> transform);

  virtual ~CompactModelBase() = default;

protected:
  Mat22 getCombinedTransform(double pixel_scale) const;

  // Jacobian transform
  Mat22 m_jacobian;
  Mat22 m_inv_jacobian;

private:
  std::shared_ptr<BasicParameter> m_x_scale;
  std::shared_ptr<BasicParameter> m_y_scale;
  std::shared_ptr<BasicParameter> m_rotation;
};

}

#include "_impl/CompactModelBase.icpp"


#endif /* _MODELFITTING_MODELS_COMPACTMODELBASE_H_ */

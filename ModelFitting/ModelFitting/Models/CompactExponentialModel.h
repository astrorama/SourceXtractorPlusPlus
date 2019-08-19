/*
 * CompactExponentialModel.h
 *
 *  Created on: Aug 19, 2019
 *      Author: mschefer
 */

#ifndef _MODELFITTING_MODELS_COMPACTEXPONENTIALMODEL_H_
#define _MODELFITTING_MODELS_COMPACTEXPONENTIALMODEL_H_

#include "ModelFitting/Models/CompactModelBase.h"

namespace ModelFitting {

template <typename ImageType>
class CompactExponentialModel : public CompactModelBase<ImageType> {

public:
  CompactExponentialModel(BasicParameter& i0, BasicParameter& k,
                BasicParameter& x_scale, BasicParameter& y_scale,
                BasicParameter& rotation, double width, double height,
                BasicParameter& x, BasicParameter& y, std::tuple<double, double, double, double> transform);

  virtual ~CompactExponentialModel() = default;

  double getValue(double x, double y) const override;
  ImageType getRasterizedImage(double pixel_scale, std::size_t size_x, std::size_t size_y) const override;

protected:
  using CompactModelBase<ImageType>::getCombinedTransform;
  using CompactModelBase<ImageType>::m_jacobian;

private:
  float evaluateModel(const Mat22& transform, float x, float y) const;

  // Sersic parameters
  double m_i0;
  ReferenceUpdater m_i0_updater;

  double m_k;
  ReferenceUpdater m_k_updater;
};

}

#include "_impl/CompactExponentialModel.icpp"

#endif /* MODELFITTING_MODELFITTING_MODELS_COMPACTEXPONENTIALMODEL_H_ */

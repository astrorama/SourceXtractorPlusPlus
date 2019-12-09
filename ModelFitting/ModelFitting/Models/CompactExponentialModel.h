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
  CompactExponentialModel(std::shared_ptr<BasicParameter> i0, std::shared_ptr<BasicParameter> k,
                std::shared_ptr<BasicParameter> x_scale, std::shared_ptr<BasicParameter> y_scale,
                std::shared_ptr<BasicParameter> rotation, double width, double height,
                std::shared_ptr<BasicParameter> x, std::shared_ptr<BasicParameter> y,
                std::tuple<double, double, double, double> transform);

  virtual ~CompactExponentialModel() = default;

  double getValue(double x, double y) const override;
  ImageType getRasterizedImage(double pixel_scale, std::size_t size_x, std::size_t size_y) const override;

private:
  using CompactModelBase<ImageType>::getCombinedTransform;
  using CompactModelBase<ImageType>::m_jacobian;

  struct EvaluateModelInfo {
    Mat22 transform;
    double i0, k;
  };

  float evaluateModel(const EvaluateModelInfo& transform, float x, float y) const;

  // Sersic parameters
  std::shared_ptr<BasicParameter> m_i0;
  std::shared_ptr<BasicParameter> m_k;
};

}

#include "_impl/CompactExponentialModel.icpp"

#endif /* MODELFITTING_MODELFITTING_MODELS_COMPACTEXPONENTIALMODEL_H_ */

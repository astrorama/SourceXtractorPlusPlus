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
  CompactExponentialModel(double sharp_radius,
                std::shared_ptr<BasicParameter> i0, std::shared_ptr<BasicParameter> k,
                std::shared_ptr<BasicParameter> x_scale, std::shared_ptr<BasicParameter> y_scale,
                std::shared_ptr<BasicParameter> rotation, double width, double height,
                std::shared_ptr<BasicParameter> x, std::shared_ptr<BasicParameter> y,
                std::shared_ptr<BasicParameter> flux,
                std::tuple<double, double, double, double> transform);

  virtual ~CompactExponentialModel() = default;

  double getValue(double x, double y) const override;
  ImageType getRasterizedImage(double pixel_scale, std::size_t size_x, std::size_t size_y) const override;

private:
  using CompactModelBase<ImageType>::getMaxRadiusSqr;
  using CompactModelBase<ImageType>::getCombinedTransform;
  using CompactModelBase<ImageType>::m_jacobian;
  using CompactModelBase<ImageType>::samplePixel;
  using CompactModelBase<ImageType>::adaptiveSamplePixel;
  using CompactModelBase<ImageType>::renormalize;

  struct ExponentialModelEvaluator {
    Mat22 transform;
    double i0, k;
    double max_r_sqr;

    inline float evaluateModel(float x, float y) const {
      float x2 = x * transform[0] + y * transform[1];
      float y2 = x * transform[2] + y * transform[3];
      float r_sqr = x2*x2 + y2*y2;
      if (r_sqr < max_r_sqr) {
        float r = std::sqrt(r_sqr);
        return float(i0) * std::exp(float(-k * r));
      } else {
        return 0;
      }
    }
  };

  float m_sharp_radius_squared;

  // Exponential parameters
  std::shared_ptr<BasicParameter> m_i0;
  std::shared_ptr<BasicParameter> m_k;
  std::shared_ptr<BasicParameter> m_flux;

};

}

#include "_impl/CompactExponentialModel.icpp"

#endif /* _MODELFITTING_MODELS_COMPACTEXPONENTIALMODEL_H_ */

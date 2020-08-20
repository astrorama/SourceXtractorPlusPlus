/*
 * CompactSersicModel.h
 *
 *  Created on: Jul 25, 2019
 *      Author: mschefer
 */

#ifndef _MODELFITTING_MODELS_COMPACTSERSICMODEL_H_
#define _MODELFITTING_MODELS_COMPACTSERSICMODEL_H_

#include "ModelFitting/Models/CompactModelBase.h"

namespace ModelFitting {

template <typename ImageType>
class CompactSersicModel : public CompactModelBase<ImageType> {

public:

  enum class SamplingMethod {
    LEGACY=0,
    ADAPTIVE=1,
    GRID=2,
    STOCHASTIC=3,
  };

  CompactSersicModel(double sharp_radius,
      std::shared_ptr<BasicParameter> i0, std::shared_ptr<BasicParameter> k, std::shared_ptr<BasicParameter> n,
      std::shared_ptr<BasicParameter> x_scale, std::shared_ptr<BasicParameter> y_scale,
      std::shared_ptr<BasicParameter> rotation, double width, double height,
      std::shared_ptr<BasicParameter> x, std::shared_ptr<BasicParameter> y,
      std::tuple<double, double, double, double> transform,
      SamplingMethod sampling_method = SamplingMethod::ADAPTIVE,
      int sample_nb = 16,
      double adaptive_target = .001
  );

  virtual ~CompactSersicModel() = default;

  double getValue(double x, double y) const override;

  ImageType getRasterizedImage(double pixel_scale, std::size_t size_x, std::size_t size_y) const override;


  struct SersicModelEvaluator {
    Mat22 transform;
    double i0, k, n;
    double max_r_sqr;

    inline float evaluateModel(float x, float y) const {
      float x2 = x * transform[0] + y * transform[1];
      float y2 = x * transform[2] + y * transform[3];
      float r_sqr = x2*x2 + y2*y2;
      if (r_sqr < max_r_sqr) {
        float r = std::sqrt(r_sqr);
        return i0 * std::exp(float(-k * powf(r, 1. / n)));
      } else {
        return 0;
      }
    }
  };

private:
  using CompactModelBase<ImageType>::getMaxRadiusSqr;
  using CompactModelBase<ImageType>::getCombinedTransform;
  using CompactModelBase<ImageType>::samplePixel;
  using CompactModelBase<ImageType>::adaptiveSamplePixel;
  using CompactModelBase<ImageType>::sampleStochastic;

  using CompactModelBase<ImageType>::m_jacobian;

  float m_sharp_radius_squared;

  // Sersic parameters
  std::shared_ptr<BasicParameter> m_i0;
  std::shared_ptr<BasicParameter> m_k;
  std::shared_ptr<BasicParameter> m_n;

  SamplingMethod m_sampling_method;
  int m_sample_nb;
  double m_adaptive_target;
};

}

#include "_impl/CompactSersicModel.icpp"

#endif /* _MODELFITTING_MODELS_COMPACTSERSICMODEL_H_ */

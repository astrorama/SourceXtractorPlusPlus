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
  CompactSersicModel(double sharp_radius,
      std::shared_ptr<BasicParameter> i0, std::shared_ptr<BasicParameter> k, std::shared_ptr<BasicParameter> n,
      std::shared_ptr<BasicParameter> x_scale, std::shared_ptr<BasicParameter> y_scale,
      std::shared_ptr<BasicParameter> rotation, double width, double height,
      std::shared_ptr<BasicParameter> x, std::shared_ptr<BasicParameter> y,
      std::tuple<double, double, double, double> transform);

  virtual ~CompactSersicModel() = default;

  double getValue(double x, double y) const override;

  ImageType getRasterizedImage(double pixel_scale, std::size_t size_x, std::size_t size_y) const override;


  struct SersicModelEvaluator {
    Mat22 transform;
    double i0, k, n;
    double max_r;

    inline float getRadius(float x, float y) const {
      float x2 = x * transform[0] + y * transform[1];
      float y2 = x * transform[2] + y * transform[3];
      return std::sqrt(x2*x2 + y2*y2);
    }

    inline float evaluateModel(float x, float y) const {
      float r = getRadius(x, y);
      return r <= max_r ? i0 * std::exp(float(-k * powf(r, 1. / n))) : 10.f;
    }
  };

private:
  double getMaxRadius(std::size_t size_x, std::size_t size_y, const Mat22& transform) const;

  using CompactModelBase<ImageType>::getCombinedTransform;
  using CompactModelBase<ImageType>::samplePixel;
  using CompactModelBase<ImageType>::adaptiveSamplePixel;

  using CompactModelBase<ImageType>::m_jacobian;

  float m_sharp_radius_squared;

  // Sersic parameters
  std::shared_ptr<BasicParameter> m_i0;
  std::shared_ptr<BasicParameter> m_k;
  std::shared_ptr<BasicParameter> m_n;
};

}

#include "_impl/CompactSersicModel.icpp"

#endif /* _MODELFITTING_MODELS_COMPACTSERSICMODEL_H_ */

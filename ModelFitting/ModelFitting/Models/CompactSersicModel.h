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

  float evaluateModel(const Mat22& transform, float x, float y) const;
  float samplePixel(const Mat22& transform, int x, int y, unsigned int subsampling) const;
  float adaptiveSamplePixel(const Mat22& transform, int x, int y, unsigned int max_subsampling, float threshold=1.1) const;

protected:
  using CompactModelBase<ImageType>::getCombinedTransform;
  using CompactModelBase<ImageType>::m_jacobian;

private:
  float m_sharp_radius_squared;

  // Sersic parameters
  std::shared_ptr<BasicParameter> m_i0;
  std::shared_ptr<BasicParameter> m_k;
  std::shared_ptr<BasicParameter> m_n;
};

}

#include "_impl/CompactSersicModel.icpp"

#endif /* _MODELFITTING_MODELS_COMPACTSERSICMODEL_H_ */

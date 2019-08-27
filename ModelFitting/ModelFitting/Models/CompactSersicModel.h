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
  CompactSersicModel(double sharp_radius, BasicParameter& i0, BasicParameter& k, BasicParameter& n,
                BasicParameter& x_scale, BasicParameter& y_scale,
                BasicParameter& rotation, double width, double height,
                BasicParameter& x, BasicParameter& y, std::tuple<double, double, double, double> transform);

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
  double m_i0;
  ReferenceUpdater m_i0_updater;

  double m_k;
  ReferenceUpdater m_k_updater;

  double m_n;
  ReferenceUpdater m_n_updater;
};

}

#include "_impl/CompactSersicModel.icpp"

#endif /* _MODELFITTING_MODELS_COMPACTSERSICMODEL_H_ */

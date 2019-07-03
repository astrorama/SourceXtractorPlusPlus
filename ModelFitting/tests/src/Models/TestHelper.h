/*
 * TestHelper.h
 *
 *  Created on: July 03, 2019
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _TEST_HELPER_H
#define _TEST_HELPER_H

#include <vector>
#include <cmath>
#include "ModelFitting/Models/ModelComponent.h"

namespace ModelFitting {

/**
 * Simple implementation for testing
 */
class LinearModelComponent : public ModelComponent {
public:
  enum Decay {
    NONE, X_AXIS, Y_AXIS, BOTH
  };

  LinearModelComponent(double center_value, double slope, Decay decay = Decay::BOTH)
    : m_center{center_value}, m_slope{slope}, m_scale{1.},
      m_r_max{std::numeric_limits<double>::infinity()}, m_decay{decay} {}

  double getValue(double x, double y) override {
    double d = 0;
    switch (m_decay) {
      case X_AXIS:
        d = std::abs(x * m_scale);
        break;
      case Y_AXIS:
        d = std::abs(y * m_scale);
        break;
      case BOTH:
        d = std::sqrt((x * x + y * y) * m_scale);
        break;
      default:
        d = 0;
    }
    if (d > m_r_max || d * m_slope > m_center) {
      return 0;
    }
    return m_center - d * m_slope;
  }

  void updateRasterizationInfo(double scale, double r_max) override {
    m_scale = scale;
    m_r_max = r_max;
  }

  std::vector<ModelSample> getSharpSampling() override {
    return {};
  }

  bool insideSharpRegion(double, double) override {
    return false;
  }

private:
  double m_center, m_slope, m_scale, m_r_max;
  Decay m_decay;
};

/**
 * Helper function to raster a model component into a vector of doubles.
 * It will use a sampling based approach, if there is any, *or* smooth otherwise.
 * It will *not* combine both. That's already provided by the ModelFitting module.
 * @see ModelFitting::ExtendedModel::getRasterizedImage
 */
std::vector<double> raster(ModelComponent& model_component, int size) {
  std::vector<double> v(size * size, 0.);
  auto sampling = model_component.getSharpSampling();

  if (sampling.size()) {
    for (auto& s: sampling) {
      ssize_t x = std::get<0>(s) + size / 2.;
      ssize_t y = std::get<1>(s) + size / 2.;
      if (x >= 0 && x < size && y >= 0 && y < size) {
        v[x + y * size] += std::get<2>(s);
      }
    }
  } else {
    for (int x = 0; x < size; ++x) {
      double x_model = x - (size - 1) / 2.;
      for (int y = 0; y < size; ++y) {
        double y_model = y - (size - 1) / 2.;
        v[x + y * size] = model_component.getValue(x_model, y_model);
      }
    }
  }

  return v;
}

double sumCentralPixels(std::vector<double> const& v, int size, int w, int h) {
  int offset_x = size / 2 - w / 2;
  int offset_y = size / 2 - h / 2;

  double sum = 0;
  for (int ix = 0; ix < w; ++ix) {
    for (int iy = 0; iy < h; ++iy) {
      sum += v[ix + offset_x + (iy + offset_y) * size];
    }
  }
  return sum;
}

} // end namespace ModelFitting

#endif // _TEST_HELPER_H

/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
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

  LinearModelComponent(double center_value, double slope, Decay decay = Decay::BOTH, int sharp_size = 0)
    : m_center{center_value}, m_slope{slope}, m_scale{1.},
      m_r_max{std::numeric_limits<double>::infinity()}, m_sharp{sharp_size}, m_decay{decay} {}

  double getValue(double x, double y) override {
    double d = 0;
    switch (m_decay) {
      case X_AXIS:
        d = std::abs(x);
        break;
      case Y_AXIS:
        d = std::abs(y);
        break;
      case BOTH:
        d = std::sqrt((x * x + y * y));
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
    m_sharp = std::min(m_sharp, static_cast<int>(m_r_max));
  }

  std::vector<ModelSample> getSharpSampling() override {
    std::vector<ModelSample> samples;
    if (m_sharp) {
      double size = m_sharp * m_scale;
      double min = std::floor(-size / 2);
      double max = std::ceil(size / 2);
      for (double x = min; x <= max; x += m_scale) {
        for (double y = min; y <= max; y += m_scale) {
          samples.emplace_back(x, y, getValue(x, y) * m_scale * m_scale);
        }
      }
    }
    return samples;
  }

  bool insideSharpRegion(double x, double y) override {
    return std::sqrt(x * x + y * y) < m_sharp;
  }

private:
  double m_center, m_slope, m_scale, m_r_max;
  int m_sharp;
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

} // end namespace ModelFitting

#endif // _TEST_HELPER_H

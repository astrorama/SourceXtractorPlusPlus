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
 * FlexibleModelFittingConverterFactory.h
 *
 *  Created on: May 10, 2019
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGCONVERTERFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGCONVERTERFACTORY_H_

#include <memory>
#include <functional>

namespace ModelFitting {
  class CoordinateConverter;
}

namespace SourceXtractor {

class SourceInterface;


class FlexibleModelFittingConverterFactory {
public:
  virtual std::unique_ptr<ModelFitting::CoordinateConverter> getConverter(double initial_value, const SourceInterface& source) const = 0;
};

class FlexibleModelFittingExponentialRangeConverterFactory : public FlexibleModelFittingConverterFactory {
public:
  /// The signature of a function providing the range of a parameter. It gets as
  /// as input the initial value of the parameter and the source, and it returns
  /// a pair containing the minimum and maximum values of the range.
  using RangeFunc = std::function<std::pair<double, double>(double, const SourceInterface&)>;

  FlexibleModelFittingExponentialRangeConverterFactory(RangeFunc range) : m_range(range) {}

  std::unique_ptr<ModelFitting::CoordinateConverter> getConverter(double initial_value, const SourceInterface& source) const override;

private:
  RangeFunc m_range;
};

class FlexibleModelFittingLinearRangeConverterFactory : public FlexibleModelFittingConverterFactory {
public:
  using RangeFunc = std::function<std::pair<double, double>(double, const SourceInterface&)>;


  FlexibleModelFittingLinearRangeConverterFactory(RangeFunc range) : m_range(range) {}

  std::unique_ptr<ModelFitting::CoordinateConverter> getConverter(double initial_value, const SourceInterface& source) const override;

private:
  RangeFunc m_range;
};

class FlexibleModelFittingUnboundedConverterFactory : public FlexibleModelFittingConverterFactory {
public:
  using ValueFunc = std::function<double(double, const SourceInterface&)>;

  FlexibleModelFittingUnboundedConverterFactory(ValueFunc normalization_factor)
      : m_normalization_factor(normalization_factor) {}

  std::unique_ptr<ModelFitting::CoordinateConverter> getConverter(double initial_value, const SourceInterface& source) const override;

private:
  ValueFunc m_normalization_factor;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGCONVERTERFACTORY_H_ */


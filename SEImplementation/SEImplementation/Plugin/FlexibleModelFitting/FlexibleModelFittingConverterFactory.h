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

namespace SExtractor {

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


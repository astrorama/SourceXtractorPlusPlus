/*
 * VariablePsf.h
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro Álvarez Ayllón
 */

#ifndef _SEIMPLEMENTATION_PSF_VARIABLEPSF_H_
#define _SEIMPLEMENTATION_PSF_VARIABLEPSF_H_

#include <SEFramework/Image/VectorImage.h>
#include "SEFramework/Property/PropertyHolder.h"

namespace SExtractor {


// TODO: Verify coefficient size
class VariablePsf {
public:
  struct Component {
    std::string name;
    int group_id;
    double offset, scale;
  };

  VariablePsf(double pixel_scale, const std::vector<Component> &components, const std::vector<int> &group_degrees,
    const std::vector<std::shared_ptr<VectorImage<SeFloat>>> &coefficients);

  VariablePsf(double pixel_scale, const std::shared_ptr<VectorImage<SeFloat>> &constant);

  virtual ~VariablePsf() = default;

  int getWidth() const;
  int getHeight() const;
  double getPixelScale() const;
  const std::vector<Component>& getComponents() const;

  std::shared_ptr<VectorImage < SeFloat>> getPsf(const std::vector<double> &prop_values) const;

private:
  double m_pixel_scale;
  std::vector<Component> m_components;
  std::vector<int> m_group_degrees;
  std::vector<std::shared_ptr<VectorImage<SeFloat>>> m_coefficients;
  std::vector<std::vector<int>> m_exponents;

  void selfTest();
  std::vector<double> scaleProperties(const std::vector<double> &prop_values) const;
  void calculateExponents();
};

}

#endif //_SEIMPLEMENTATION_PSF_VARIABLEPSF_H_

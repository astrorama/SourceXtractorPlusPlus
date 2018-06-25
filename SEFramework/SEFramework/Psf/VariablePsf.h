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
// TODO: Load form PsfEx file
class VariablePsf {
public:
  VariablePsf(const std::vector<unsigned> &property_groups, const std::vector<unsigned> &group_degrees,
    const std::vector<double> &offsets, const std::vector<double> &scales,
    const std::vector<std::shared_ptr<VectorImage<double>>> &coefficients);

  virtual ~VariablePsf() = default;

  int getWidth() const;
  int getHeight() const;

  std::shared_ptr<VectorImage<double>> getPsf(const std::vector<double> &prop_values) const;

public:
  std::vector<unsigned> m_property_groups, m_group_degrees;
  std::vector<double> m_offsets, m_scales;
  std::vector<std::shared_ptr<VectorImage<double>>> m_coefficients;
  std::vector<std::vector<unsigned>> m_exponents;

  void selfTest();
  std::vector<double> scaleProperties(const std::vector<double> &prop_values) const;
  void calculateExponents();
};

}

#endif //_SEIMPLEMENTATION_PSF_VARIABLEPSF_H_

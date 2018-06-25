/*
 * VariablePsf.cpp
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro Álvarez Ayllón
 */

#include <ElementsKernel/Exception.h>
#include <algorithm>
#include "SEFramework/Psf/VariablePsf.h"


namespace SExtractor {

VariablePsf::VariablePsf(const std::vector<unsigned> &property_groups, const std::vector<unsigned> &group_degrees,
            const std::vector<double> &offsets, const std::vector<double> &scales,
            const std::vector<std::shared_ptr<VectorImage<double>>> &coefficients):
  m_property_groups(property_groups), m_group_degrees(group_degrees),
  m_offsets(offsets), m_scales(scales), m_coefficients(coefficients)
{
  selfTest();
  calculateExponents();
}

int VariablePsf::getWidth() const {
  return m_coefficients[0]->getWidth();
}

int VariablePsf::getHeight() const {
  return m_coefficients[0]->getHeight();
}

std::shared_ptr<VectorImage<double>> VariablePsf::getPsf(const std::vector<double> &prop_values) const
{
  auto scaled_props = scaleProperties(prop_values);
  // Initialize with the constant component
  auto result = VectorImage<double>::create(*m_coefficients[0]);

  // Add the rest of the components
  for (auto i = 1; i < m_coefficients.size(); ++i) {
    const auto &exp = m_exponents[i];
    const auto &coef = m_coefficients[i];

    double acc = 1.;
    for (auto j = 0; j < scaled_props.size(); ++j) {
      acc *= std::pow(scaled_props[j], exp[j]);
    }

    for (auto x = 0; x < getWidth(); ++x) {
      for (auto y = 0; y < getHeight(); ++y) {
        result->at(x, y) += acc * coef->at(x, y);
      }
    }
  }

  return result;
}

void VariablePsf::selfTest() {
  if (m_coefficients.size() == 0) {
    throw Elements::Exception() << "A variable PSF needs at least one set of coefficients";
  }

  if (m_property_groups.size()) {
    auto max_group_id = *std::max_element(m_property_groups.begin(), m_property_groups.end());
    if (max_group_id != m_group_degrees.size() - 1) {
      throw Elements::Exception()
          << "Number of groups and number of group degrees must match, got max group id "
          << max_group_id << " and " << m_group_degrees.size() << " group degrees";
    }
  }

  if (m_offsets.size() != m_scales.size()) {
    throw Elements::Exception()
        << "Expecting same number of offsets and scales, got " << m_offsets.size() << " offsets and "
        << m_scales.size() << " scales";

  }

  std::vector<unsigned> n_props_per_group(m_group_degrees.size());
  for (auto prop_group : m_property_groups) {
    ++n_props_per_group[prop_group];
  }

  auto psf_width = m_coefficients[0]->getWidth();
  auto psf_height = m_coefficients[0]->getHeight();

  for (auto coeff : m_coefficients) {
    if (coeff->getWidth() != psf_width || coeff->getHeight() != psf_height) {
      throw Elements::Exception() << "Malformed variable PSF, coefficient matrices do not have the same dimensions";
    }
  }
}

std::vector<double> VariablePsf::scaleProperties(const std::vector<double> &prop_values) const
{
  if (prop_values.size() != m_offsets.size()) {
    throw Elements::Exception()
        << "Expecting " << m_offsets.size() << " values, got " << prop_values.size();
  }
  std::vector<double> scaled(prop_values.size());
  for (auto i = 0; i < prop_values.size(); ++i) {
    scaled[i] = (prop_values[i] - m_offsets[i]) / m_scales[i];
  }
  return scaled;
}

void VariablePsf::calculateExponents() {
  std::vector<unsigned> group_exponents(m_group_degrees);
  std::vector<unsigned> exp(m_property_groups.size());

  m_exponents.resize(m_coefficients.size());

  // Constant
  m_exponents[0].resize(m_property_groups.size());
  if (m_property_groups.size() > 0) {
    --group_exponents[m_property_groups[0]];
  }
  else {
    return;
  }

  // Polynomial
  exp[0] = 1;
  for (auto e = m_exponents.begin() + 1; e != m_exponents.end(); ++e) {
    *e = exp;

    size_t ei = 0;
    for (auto prop_group : m_property_groups) {
      if (group_exponents[prop_group] > 0) {
        --group_exponents[prop_group];
        ++exp[ei];
        break;
      }
      else {
        group_exponents[prop_group] = exp[ei];
        exp[ei++] = 0;
      }
    }
  }
}

} // end SExtractor
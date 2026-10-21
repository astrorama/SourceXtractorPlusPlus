/** Copyright © 2019-2026 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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
#ifndef _SEIMPLEMENTATION_PLUGIN_SPREADMODEL_COMBINEDSPREADMODEL_H_
#define _SEIMPLEMENTATION_PLUGIN_SPREADMODEL_COMBINEDSPREADMODEL_H_

#include "SEFramework/Property/Property.h"

namespace SourceXtractor {

/** Inverse-variance combination of valid per-frame spread measurements.
 * The error assumes independent frames; reduced chi-square diagnoses disagreement
 * without inflating the error or rejecting outliers. Undefined quantities are NaN.
 */
class CombinedSpreadModel : public Property {
public:
  CombinedSpreadModel(double value, double error, unsigned int n_frames, double reduced_chi_squared)
      : m_value(value), m_error(error), m_n_frames(n_frames), m_reduced_chi_squared(reduced_chi_squared) {}

  double getSpreadModel() const { return m_value; }
  double getSpreadModelError() const { return m_error; }
  unsigned int getNFrames() const { return m_n_frames; }
  double getReducedChiSquared() const { return m_reduced_chi_squared; }

private:
  double m_value;
  double m_error;
  unsigned int m_n_frames;
  double m_reduced_chi_squared;
};

} // namespace SourceXtractor
#endif

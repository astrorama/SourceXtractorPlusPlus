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
 * PeakValue.h
 *
 *  Created on: Feb 9, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_PEAKVALUE_PEAKVALUE_H_
#define _SEIMPLEMENTATION_PLUGIN_PEAKVALUE_PEAKVALUE_H_

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"

namespace SourceXtractor {

class PeakValue : public Property {
public:

  virtual ~PeakValue() = default;

  PeakValue(SeFloat min_value, SeFloat max_value, int max_value_x, int max_value_y) : m_min_value(min_value), m_max_value(max_value), m_peak_x(max_value_x), m_peak_y(max_value_y) {}

  SeFloat getMinValue() const {
    return m_min_value;
  }

  SeFloat getMaxValue() const {
    return m_max_value;
  }

  SeFloat getMaxValueX() const {
    return m_peak_x;
  }

  SeFloat getMaxValueY() const {
    return m_peak_y;
  }

private:
  SeFloat m_min_value, m_max_value;
  int m_peak_x, m_peak_y;

};

} /* namespace SourceXtractor */





#endif /* SEIMPLEMENTATION_SEIMPLEMENTATION_PLUGIN_PEAKVALUE_PEAKVALUE_H_ */

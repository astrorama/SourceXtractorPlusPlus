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

namespace SExtractor {

class PeakValue : public Property {
public:

  virtual ~PeakValue() = default;

  PeakValue(SeFloat min_value, SeFloat max_value) : m_min_value(min_value), m_max_value(max_value) {}

  SeFloat getMinValue() const {
    return m_min_value;
  }

  SeFloat getMaxValue() const {
    return m_max_value;
  }

private:
  SeFloat m_min_value, m_max_value;

};

} /* namespace SExtractor */





#endif /* SEIMPLEMENTATION_SEIMPLEMENTATION_PLUGIN_PEAKVALUE_PEAKVALUE_H_ */

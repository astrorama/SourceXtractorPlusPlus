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

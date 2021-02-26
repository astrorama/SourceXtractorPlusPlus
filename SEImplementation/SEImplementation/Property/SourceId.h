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
 * SourceId.h
 *
 *  Created on: Apr 26, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PROPERTY_SOURCEID_H_
#define _SEIMPLEMENTATION_PROPERTY_SOURCEID_H_

#include "SEFramework/Property/Property.h"
#include <atomic>

namespace SourceXtractor {

class SourceId : public Property {

public:

  SourceId(unsigned int detection_id)
      : m_source_id(getNewId()), m_detection_id(detection_id) {
  }

  SourceId()
      : m_source_id(getNewId()), m_detection_id(m_source_id) {
  }

  virtual ~SourceId() = default;

  unsigned int getSourceId() const {
    return m_source_id;
  }

  unsigned int getDetectionId() const {
    return m_detection_id;
  }

private:
  unsigned int m_source_id, m_detection_id;

  static unsigned int getNewId() {
    static std::atomic<std::uint32_t> s_id(1);
    return s_id++;
  }



}; /* End of SourceId class */

}


#endif /* _SEIMPLEMENTATION_PROPERTY_SOURCEID_H_ */

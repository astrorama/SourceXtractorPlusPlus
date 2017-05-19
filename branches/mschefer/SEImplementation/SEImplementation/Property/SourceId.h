/*
 * SourceId.h
 *
 *  Created on: Apr 26, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PROPERTY_SOURCEID_H_
#define _SEIMPLEMENTATION_PROPERTY_SOURCEID_H_

#include "SEFramework/Property/Property.h"

namespace SExtractor {

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
    static unsigned int s_id  = 0;
    return s_id++;
  }



}; /* End of SourceId class */

}


#endif /* _SEIMPLEMENTATION_PROPERTY_SOURCEID_H_ */

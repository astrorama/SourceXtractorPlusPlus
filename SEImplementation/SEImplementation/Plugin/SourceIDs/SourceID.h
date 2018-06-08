/*
 * SourceID.h
 *
 *  Created on: May 30, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEID_H_
#define _SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEID_H_


#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"

namespace SExtractor {

class SourceID : public Property {
public:

  virtual ~SourceID() = default;

  SourceID(unsigned int id, unsigned int detection_id) : m_id(id), m_detection_id(detection_id) {}

  int  getId() const {
    return m_id;
  }

  int  getDetectionId() const {
    return m_detection_id;
  }


private:
  unsigned int m_id;
  unsigned int m_detection_id;
};

} /* namespace SExtractor */



#endif /* _SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEID_H_ */

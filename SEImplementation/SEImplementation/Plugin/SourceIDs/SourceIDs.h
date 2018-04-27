/*
 * SourcesIDs.h
 *
 *  Created on: Apr 27, 2018
 *      Author: mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEIDS_H_
#define _SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEIDS_H_

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"

namespace SExtractor {

/**
 * @class SourceIDs
 * @brief SourceIDs segmentation ID partition ID
 */
class SourceIDs : public Property {
public:

  /**
   * @brief Destructor
   */
  virtual ~SourceIDs() = default;

  SourceIDs()
    :m_part_ID(0), m_segm_ID(0) {}

  long getPartID() const {
    return m_part_ID;
  }

  long getSegmID() const {
    return m_segm_ID;
  }

private:
  long m_part_ID;
  long m_segm_ID;
};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_PLUGIN_SOURCESIDS_SOURCESIDS_H_ */

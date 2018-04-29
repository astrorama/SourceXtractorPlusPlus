/*
 * Copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General
 * Public License as published by the Free Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
 * SourcesIDs.h
 *
 *  Created on: Apr 27, 2018
 *      Author: mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEIDS_H_
#define _SEIMPLEMENTATION_PLUGIN_SOURCEIDS_SOURCEIDS_H_

#include "SEFramework/Property/Property.h"

namespace SExtractor {
/**
 * @class SourceIDs
 * @brief SourceIDs segmentation ID partition ID
 */
class SourceIDs : public Property {
public:
  virtual ~SourceIDs() = default;
  SourceIDs(unsigned int source_id, unsigned int detect_id) : m_source_id(source_id), m_detect_id(detect_id) {}

  unsigned int getSourceID() const {
    return m_source_id;
  }

  unsigned int getDetectID() const {
    return m_detect_id;
  }

private:
  unsigned int m_source_id;
  unsigned int m_detect_id;
};
} /* namespace SExtractor */
#endif /* _SEIMPLEMENTATION_PLUGIN_SOURCESIDS_SOURCESIDS_H_ */

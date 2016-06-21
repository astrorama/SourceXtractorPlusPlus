/**
 * @file SEImplementation/Grouping/OverlappingBoundariesCriteria.h
 * @date 06/07/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_GROUPING_OVERLAPPINGBOUNDARIESCRITERIA_H
#define _SEIMPLEMENTATION_GROUPING_OVERLAPPINGBOUNDARIESCRITERIA_H

#include "SEFramework/Pipeline/SourceGrouping.h"

namespace SExtractor {

/**
 * @class OverlappingBoundariesCriteria
 * @brief Groups sources if their bounding boxes overlap (to be used with OverlappingBoundariesSourceList)
 *
 */

class OverlappingBoundariesCriteria : public GroupingCriteria {
public:
  virtual bool shouldGroup(const SourceList& source_list, const Source& source) const override;

};


} /* namespace SExtractor */


#endif

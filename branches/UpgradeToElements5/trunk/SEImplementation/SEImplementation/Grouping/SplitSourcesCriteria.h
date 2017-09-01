/*
 * SplitSourcesCriteria.h
 *
 *  Created on: Apr 26, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_GROUPING_SPLITSOURCESCRITERIA_H_
#define _SEIMPLEMENTATION_GROUPING_SPLITSOURCESCRITERIA_H_

#include "SEFramework/Pipeline/SourceGrouping.h"

namespace SExtractor {

/**
 * @class SplitSourcesCriteria
 * @brief Groups sources if they were originally one source before being split
 *
 */

class SplitSourcesCriteria : public GroupingCriteria {
public:
  virtual bool shouldGroup(const SourceInterface& first, const SourceInterface& second) const override;
};


} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_GROUPING_SPLITSOURCESCRITERIA_H_ */

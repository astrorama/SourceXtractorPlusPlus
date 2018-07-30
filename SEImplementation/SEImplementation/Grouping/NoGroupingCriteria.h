/*
 * NoGroupingCriteria.h
 *
 *  Created on: Jul 2, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_GROUPING_NOGROUPINGCRITERIA_H_
#define _SEIMPLEMENTATION_GROUPING_NOGROUPINGCRITERIA_H_

#include "SEFramework/Pipeline/SourceGrouping.h"

namespace SExtractor {


class NoGroupingCriteria : public GroupingCriteria {
public:
  virtual bool shouldGroup(const SourceInterface&, const SourceInterface&) const override {
    return false;
  }
};


} /* namespace SExtractor */




#endif /* _SEIMPLEMENTATION_GROUPING_NOGROUPINGCRITERIA_H_ */

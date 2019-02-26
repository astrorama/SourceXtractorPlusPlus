/*
 * MoffatCritteria.h
 *
 *  Created on: 2019 M01 25
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_GROUPING_MOFFATCRITERIA_H_
#define _SEIMPLEMENTATION_GROUPING_MOFFATCRITERIA_H_

#include "SEFramework/Pipeline/SourceGrouping.h"

namespace SExtractor {

class MoffatModelFitting;

/**
 * @class MoffatCriteria
 * @brief Groups sources if their Moffat profile overlap
 *
 */

class MoffatCriteria : public GroupingCriteria {
public:

  MoffatCriteria(double threshold) : m_threshold(threshold) {}
  virtual ~MoffatCriteria() {}

  virtual bool shouldGroup(const SourceInterface&, const SourceInterface&) const override;

private:
  bool doesImpact(const SourceInterface& impactor, const SourceInterface& impactee) const;

  double m_threshold;
};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_GROUPING_MOFFATCRITERIA_H_ */

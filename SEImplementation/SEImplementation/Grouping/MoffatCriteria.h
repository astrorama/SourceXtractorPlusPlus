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
 * MoffatCritteria.h
 *
 *  Created on: 2019 M01 25
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_GROUPING_MOFFATCRITERIA_H_
#define _SEIMPLEMENTATION_GROUPING_MOFFATCRITERIA_H_

#include "SEFramework/Pipeline/SourceGrouping.h"

namespace SourceXtractor {

class MoffatModelFitting;

/**
 * @class MoffatCriteria
 * @brief Groups sources if their Moffat profile overlap
 *
 */

class MoffatCriteria : public GroupingCriteria {
public:

  MoffatCriteria(double threshold, double max_distance) : m_threshold(threshold), m_max_distance(max_distance) {}
  virtual ~MoffatCriteria() {}

  bool shouldGroup(const SourceInterface&, const SourceInterface&) const override;

  std::set<PropertyId> requiredProperties() const override;

private:
  bool doesImpact(const SourceInterface& impactor, const SourceInterface& impactee) const;

  double m_threshold;
  double m_max_distance;
};

} /* namespace SourceXtractor */

#endif /* _SEIMPLEMENTATION_GROUPING_MOFFATCRITERIA_H_ */

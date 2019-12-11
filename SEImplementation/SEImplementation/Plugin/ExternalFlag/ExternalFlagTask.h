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
/**
 * @file SEImplementation/Task/ExternalFlagTask.h
 * @date 06/17/16
 * @author nikoapos
 */

#ifndef _SEIMPLEMENTATION_TASK_EXTERNALFLAGTASK_H
#define _SEIMPLEMENTATION_TASK_EXTERNALFLAGTASK_H

#include "SEFramework/Task/SourceTask.h"
#include "SEFramework/Image/Image.h"
#include "SEFramework/Source/SourceWithOnDemandProperties.h"
#include "SEImplementation/Plugin/ExternalFlag/ExternalFlag.h"

namespace SourceXtractor {

namespace ExternalFlagCombineTypes {
struct Or;
struct And;
struct Min;
struct Max;
struct Most;
}

/**
 * @class ExternalFlagTask
 * @brief Task to produce the ExternalFlag Property
 *
 */
template <typename Combine>
class ExternalFlagTask : public SourceTask {
  
public:
  
  virtual ~ExternalFlagTask() = default;
  
  ExternalFlagTask(std::shared_ptr<FlagImage> flag_image, unsigned int flag_instance);

  void computeProperties(SourceInterface& source) const override;

private:
  
  std::shared_ptr<FlagImage> m_flag_image;
  unsigned int m_flag_instance;
  
};

extern template class ExternalFlagTask<ExternalFlagCombineTypes::Or>;
using ExternalFlagTaskOr = ExternalFlagTask<ExternalFlagCombineTypes::Or>;

extern template class ExternalFlagTask<ExternalFlagCombineTypes::And>;
using ExternalFlagTaskAnd = ExternalFlagTask<ExternalFlagCombineTypes::And>;

extern template class ExternalFlagTask<ExternalFlagCombineTypes::Min>;
using ExternalFlagTaskMin = ExternalFlagTask<ExternalFlagCombineTypes::Min>;

extern template class ExternalFlagTask<ExternalFlagCombineTypes::Max>;
using ExternalFlagTaskMax = ExternalFlagTask<ExternalFlagCombineTypes::Max>;

extern template class ExternalFlagTask<ExternalFlagCombineTypes::Most>;
using ExternalFlagTaskMost = ExternalFlagTask<ExternalFlagCombineTypes::Most>;

} /* namespace SourceXtractor */


#endif

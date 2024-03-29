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
 * @file DeblendStepConfg.h
 * @author nikoapos
 */

#ifndef _SEIMPLEMENTATION_DEBLENDSTEPCONFG_H
#define _SEIMPLEMENTATION_DEBLENDSTEPCONFG_H

#include <vector>
#include <functional>

#include "Configuration/Configuration.h"

#include "SEFramework/Pipeline/Deblending.h"
#include "SEFramework/Source/SourceFactory.h"

namespace SourceXtractor {

class DeblendStepConfig : public Euclid::Configuration::Configuration {
  
public:
  
  using DeblendStepCreator = std::function<std::shared_ptr<DeblendStep>(std::shared_ptr<SourceFactory>)>;
  
  explicit DeblendStepConfig(long manager_id);
  
  virtual ~DeblendStepConfig() = default;
  
  void addDeblendStepCreator(DeblendStepCreator step_creator);
  
  std::vector<std::shared_ptr<DeblendStep>> getSteps(std::shared_ptr<SourceFactory> source_factory) const;
  
private:
  
  std::vector<DeblendStepCreator> m_deblend_step_creators;
  
};

} /* namespace SourceXtractor */

#endif /* _SEIMPLEMENTATION_DEBLENDSTEPCONFG_H */


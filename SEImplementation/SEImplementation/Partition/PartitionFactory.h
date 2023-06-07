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
 * @file PartitionFactory.h
 * @author nikoapos
 */

#ifndef _SEIMPLEMENTATION_PARTITIONFACTORY_H
#define _SEIMPLEMENTATION_PARTITIONFACTORY_H

#include "SEFramework/Configuration/Configurable.h"
#include "SEFramework/Pipeline/Partition.h"
#include "SEFramework/Source/SourceFactory.h"

#include "SEImplementation/Configuration/PartitionStepConfig.h"

namespace SourceXtractor {

class PartitionFactory : public Configurable {
  
public:
  
  explicit PartitionFactory(std::shared_ptr<SourceFactory> source_factory) :m_source_factory{source_factory} {
  }
  
  virtual ~PartitionFactory() = default;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override {
    manager.registerConfiguration<PartitionStepConfig>();
  }

  void configure(Euclid::Configuration::ConfigManager& manager) override {
    m_steps = manager.getConfiguration<PartitionStepConfig>().getSteps(m_source_factory);
  }

  std::shared_ptr<Partition> getPartition() const {
    return std::make_shared<Partition>(m_steps);
  }
  
private:
  
  std::shared_ptr<SourceFactory> m_source_factory;
  std::vector<std::shared_ptr<PartitionStep>> m_steps;

};

} /* namespace SourceXtractor */

#endif /* _SEIMPLEMENTATION_PARTITIONFACTORY_H */


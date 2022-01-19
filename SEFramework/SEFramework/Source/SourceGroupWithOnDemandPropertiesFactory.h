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
 * @file SourceGroupWithOnDemandPropertiesFactory.h
 * @author nikoapos
 */

#ifndef _SEFRAMEWORK_SOURCEGROUPWITHONDEMANDPROPERTIESFACTORY_H
#define _SEFRAMEWORK_SOURCEGROUPWITHONDEMANDPROPERTIESFACTORY_H

#include "SEFramework/Source/SourceGroupFactory.h"
#include "SEFramework/Source/SourceGroupWithOnDemandProperties.h"

namespace SourceXtractor {

/**
 * @class SourceGroupWithOnDemandPropertiesFactory
 * @brief A SourceGroupFactory implementation that produces a SourceGroupWithOnDemandProperties
 */

class SourceGroupWithOnDemandPropertiesFactory : public SourceGroupFactory {
  
public:
  
  explicit SourceGroupWithOnDemandPropertiesFactory(std::shared_ptr<TaskProvider> task_provider) :
        m_task_provider(task_provider) {}

  virtual std::shared_ptr<SourceGroupInterface> createSourceGroup() const override {
    return std::make_shared<SourceGroupWithOnDemandProperties>(m_task_provider);
  }
  
private:
  
  std::shared_ptr<TaskProvider> m_task_provider;
  
};

}

#endif /* _SEFRAMEWORK_SOURCEGROUPWITHONDEMANDPROPERTIESFACTORY_H */


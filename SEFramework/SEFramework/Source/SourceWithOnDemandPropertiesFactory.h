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
 * SourceWithOnDemandPropertiesFactory.h
 *
 *  Created on: Aug 8, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_SOURCE_SOURCEWITHONDEMANDPROPERTIESFACTORY_H_
#define _SEFRAMEWORK_SOURCE_SOURCEWITHONDEMANDPROPERTIESFACTORY_H_

#include "SEFramework/Source/SourceFactory.h"
#include "SEFramework/Source/SourceWithOnDemandProperties.h"

namespace SExtractor {

/**
 * @class SourceWithOnDemandPropertiesFactory
 * @brief A SourceFactory implementation that produces a SourceWithOnDemandProperties
 */

class SourceWithOnDemandPropertiesFactory : public SourceFactory {
public:
  SourceWithOnDemandPropertiesFactory(std::shared_ptr<TaskProvider> task_provider) :
        m_task_provider(task_provider) {}

  virtual std::shared_ptr<SourceInterface> createSource() const override {
    return std::make_shared<SourceWithOnDemandProperties>(m_task_provider);
  }

private:
  std::shared_ptr<TaskProvider> m_task_provider;
};

}


#endif /* _SEFRAMEWORK_SOURCE_SOURCEWITHONDEMANDPROPERTIESFACTORY_H_ */

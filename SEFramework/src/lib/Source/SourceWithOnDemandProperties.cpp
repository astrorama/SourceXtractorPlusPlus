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
 * @file src/lib/Source/SourceWithOnDemandProperties.cpp
 * @date 05/04/16
 * @author mschefer
 */

#include "ElementsKernel/Logging.h"

#include "SEFramework/Task/TaskProvider.h"
#include "SEFramework/Task/SourceTask.h"
#include "SEFramework/Property/PropertyNotFoundException.h"

#include "SEFramework/Source/SourceWithOnDemandProperties.h"

namespace SourceXtractor {

static Elements::Logging logger = Elements::Logging::getLogger("SourceWithOnDemandProperties");

SourceWithOnDemandProperties::SourceWithOnDemandProperties(std::shared_ptr<const TaskProvider> task_provider) :
            m_task_provider(task_provider) {
}

const Property& SourceWithOnDemandProperties::getProperty(const PropertyId& property_id) const {
  // if we have the property already, just return it
  if (m_property_holder.isPropertySet(property_id)) {
    return m_property_holder.getProperty(property_id);
  }

  try {
    // if not, get the task that makes it and execute, we should have it then
    auto task = m_task_provider->getTask<SourceTask>(property_id);
    if (task) {
      task->computeProperties(const_cast<SourceWithOnDemandProperties&>(*this));
      return m_property_holder.getProperty(property_id);
    }
  }
  catch (Elements::Exception& e) {
    logger.debug() << e.what();
  }

  // no task available to make the property, just throw an exception
  throw PropertyNotFoundException(property_id);
}

void SourceWithOnDemandProperties::setProperty(std::unique_ptr<Property> property, const PropertyId& property_id) {
  // just forward to the ObjectWithProperties implementation
  m_property_holder.setProperty(std::move(property), property_id);
}


} // SEFramework namespace


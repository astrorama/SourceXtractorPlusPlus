/**
 * @file src/lib/Source/SourceWithOnDemandProperties.cpp
 * @date 05/04/16
 * @author mschefer
 */

#include "SEFramework/Task/TaskProvider.h"
#include "SEFramework/Task/SourceTask.h"
#include "SEFramework/Property/PropertyNotFoundException.h"

#include "SEFramework/Source/SourceWithOnDemandProperties.h"

namespace SExtractor {

SourceWithOnDemandProperties::SourceWithOnDemandProperties(std::shared_ptr<const TaskProvider> task_provider) :
            m_task_provider(task_provider) {
}

const Property& SourceWithOnDemandProperties::getProperty(const PropertyId& property_id) const {
  // if we have the property already, just return it
  if (m_property_holder.isPropertySet(property_id)) {
    return m_property_holder.getProperty(property_id);
  }

  // if not, get the task that makes it and execute, we should have it then
  auto task = m_task_provider->getTask<SourceTask>(property_id);
  if (task) {
    task->computeProperties(const_cast<SourceWithOnDemandProperties&>(*this));
    return m_property_holder.getProperty(property_id);
  }

  // no task available to make the property, just throw an exception
  throw PropertyNotFoundException(property_id);
}

void SourceWithOnDemandProperties::setProperty(std::unique_ptr<Property> property, const PropertyId& property_id) {
  // just forward to the ObjectWithProperties implementation
  m_property_holder.setProperty(std::move(property), property_id);
}


} // SEFramework namespace


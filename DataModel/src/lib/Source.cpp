/**
 * @file src/lib/Source.cpp
 * @date 05/04/16
 * @author mschefer
 */

#include "DataModel/Source.h"
#include "DataModel/TaskRegistry.h"
#include "DataModel/SourceTask.h"
#include "DataModel/PropertyNotFoundException.h"


namespace DataModel {

Source::Source(std::shared_ptr<TaskRegistry> task_registry) :
    m_task_registry(task_registry) {
}

Property& Source::getPropertyImpl(const PropertyId property_id) const {
  // if we have the property already, just return it
  if (isPropertySet(property_id)) {
    return ObjectWithProperties::getPropertyImpl(property_id);
  }

  // if not, get the task that makes it and execute, we should have it then
  auto task = m_task_registry->getTask<SourceTask>(property_id);
  if (task) {
    task->computeProperties(const_cast<Source&>(*this));
    return ObjectWithProperties::getPropertyImpl(property_id);
  }

  // no task available to make the property, just throw an exception
  throw PropertyNotFoundException();
}

void Source::setPropertyImpl(std::unique_ptr<Property> property, PropertyId property_id) {
  // just forward to the ObjectWithProperties implementation
  ObjectWithProperties::setPropertyImpl(std::move(property), property_id);
}

} // DataModel namespace


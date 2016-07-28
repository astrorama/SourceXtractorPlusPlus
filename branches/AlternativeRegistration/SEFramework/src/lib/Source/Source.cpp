/**
 * @file src/lib/Source/Source.cpp
 * @date 05/04/16
 * @author mschefer
 */

#include "SEFramework/Task/TaskRegistry.h"
#include "SEFramework/Task/SourceTask.h"
#include "SEFramework/Property/PropertyNotFoundException.h"

#include "SEFramework/Source/Source.h"

namespace SExtractor {

Source::Source(std::vector<PixelCoordinate> pixels, std::shared_ptr<const TaskRegistry> task_registry) :
    PixelSourceInterface(std::move(pixels)), m_task_registry(task_registry) {
}

const Property& Source::getProperty(const PropertyId& property_id) const {
  // if we have the property already, just return it
  if (m_property_holder.isPropertySet(property_id)) {
    return m_property_holder.getProperty(property_id);
  }

  // if not, get the task that makes it and execute, we should have it then
  auto task = m_task_registry->getTask<SourceTask>(property_id);
  if (task) {
    task->computeProperties(const_cast<Source&>(*this));
    return m_property_holder.getProperty(property_id);
  }

  // no task available to make the property, just throw an exception
  throw PropertyNotFoundException();
}

void Source::setProperty(std::unique_ptr<Property> property, const PropertyId& property_id) {
  // just forward to the ObjectWithProperties implementation
  m_property_holder.setProperty(std::move(property), property_id);
}


} // SEFramework namespace


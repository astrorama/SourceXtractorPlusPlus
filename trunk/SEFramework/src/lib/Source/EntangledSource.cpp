/* 
 * @file EntangledSource.cpp
 * @author nikoapos
 */

#include "SEFramework/Source/SourceGroupWithOnDemandProperties.h"
#include "SEFramework/Task/GroupTask.h"

namespace SExtractor {

SourceGroupWithOnDemandProperties::EntangledSource::EntangledSource(std::shared_ptr<SourceInterface> source, SourceGroupWithOnDemandProperties& group)
        : m_source(source), m_group(group) {
  // Normally, it should not be possible that the given source is of type
  // EntangledSource, because the entangled sources of a group can only be
  // accessed via the iterator as references. Nevertheless, to be sure that
  // future changes will not change the behavior, we do a check to the given
  // source and if it is an EntangledSource we use its encapsulated source instead.
  auto entangled_ptr = std::dynamic_pointer_cast<EntangledSource>(m_source);
  if (entangled_ptr != nullptr) {
    m_source = entangled_ptr->m_source;
  }
}

const Property& SourceGroupWithOnDemandProperties::EntangledSource::getProperty(const PropertyId& property_id) const {

  // If we already have the property stored in this object, returns it
  if (m_property_holder.isPropertySet(property_id)) {
    return m_property_holder.getProperty(property_id);
  }

  // If the property is already stored in the group, we return it
  if (m_group.m_property_holder.isPropertySet(property_id)) {
    return m_group.getProperty(property_id);
  }

  try {
    // Try to get the the property from the encapsulated Source
    // if it cannot provide it, this will throw a PropertyNotFoundException
    return getPropertyFromInterface(*m_source, property_id);
  } catch (PropertyNotFoundException& e) {
    // Getting this exception means the property must be computed at the group level

    // Get the group task
    auto group_task = m_group.m_task_provider->getTask<GroupTask>(property_id);
    if (!group_task) {
      // No task is available to make that property
      throw PropertyNotFoundException();
    }

    // Use the task to make the property
    group_task->computeProperties(m_group);

    // The property should now be available either in this object or in the group object
    if (m_property_holder.isPropertySet(property_id)) {
      return m_property_holder.getProperty(property_id);
    } else {
      return m_group.m_property_holder.getProperty(property_id);
    }
  }

} // end of getProperty()

void SourceGroupWithOnDemandProperties::EntangledSource::setProperty(std::unique_ptr<Property> property, const PropertyId& property_id) {
  m_property_holder.setProperty(std::move(property), property_id);
}

bool SourceGroupWithOnDemandProperties::EntangledSource::operator<(const EntangledSource& other) const {
  return this->m_source < other.m_source;
}

} // SExtractor namespace
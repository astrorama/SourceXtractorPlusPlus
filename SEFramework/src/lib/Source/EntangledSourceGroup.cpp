/**
 * @file src/lib/Source/EntangledSourceGroup.cpp
 * @date 05/09/16
 * @author mschefer
 */

#include "SEFramework/Task/TaskRegistry.h"
#include "SEFramework/Task/GroupTask.h"
#include "SEFramework/Source/Source.h"
#include "SEFramework/Property/PropertyNotFoundException.h"

#include "SEFramework/Source/EntangledSourceGroup.h"

namespace SExtractor {

EntangledSourceGroup::EntangledSourceGroup(std::shared_ptr<TaskRegistry> task_registry)
    : m_task_registry(task_registry) {}

void EntangledSourceGroup::setSources(std::list<std::shared_ptr<Source>> sources) {
  m_sources.reserve(sources.size());
  for (auto& source : sources) {
    // Encapsulates every Source into an EntangledSource object and stores it
    m_sources.push_back(std::make_shared<EntangledSource>(source, shared_from_this(), m_task_registry));
  }
}

std::vector<std::shared_ptr<const SourceInterface>> EntangledSourceGroup::getSources() const {
  std::vector<std::shared_ptr<const SourceInterface>> sources;
  sources.reserve(m_sources.size());
  for (auto source : m_sources) {
    // Encapsulates every EntangledSource into an EntangledSourceWrapper before returning it
    sources.push_back(std::make_shared<EntangledSourceWrapper>(source, shared_from_this()));
  }
  return sources;
}

std::vector<std::shared_ptr<SourceInterface>> EntangledSourceGroup::getSources() {
  std::vector<std::shared_ptr<SourceInterface>> sources;
  sources.reserve(m_sources.size());
  for (auto source : m_sources) {
    // Encapsulates every EntangledSource into an EntangledSourceWrapper before returning it
    sources.push_back(std::make_shared<EntangledSourceWrapper>(source, shared_from_this()));
  }
  return sources;
}

const Property& EntangledSourceGroup::getProperty(const PropertyId& property_id) const {
  // If we already have the property, return it
  if (m_property_holder.isPropertySet(property_id)) {
    return m_property_holder.getProperty(property_id);
  }

  // If not, get the task for that property, use it to compute the property then return it
  auto task = m_task_registry->getTask<GroupTask>(property_id);
  if (task) {
    task->computeProperties(const_cast<EntangledSourceGroup&>(*this));
    return m_property_holder.getProperty(property_id);
  }

  // No task available to make that property, we throw an exception
  throw PropertyNotFoundException();
}

void EntangledSourceGroup::setProperty(std::unique_ptr<Property> property, const PropertyId& property_id) {
  m_property_holder.setProperty(std::move(property), property_id);
}

/*********************************************************************************************************************/

EntangledSourceGroup::EntangledSource::EntangledSource(std::shared_ptr<Source> source,
    std::shared_ptr<EntangledSourceGroup> group, std::shared_ptr<TaskRegistry> task_registry)
      : m_source(source), m_group(group), m_task_registry(task_registry) {}

const Property& EntangledSourceGroup::EntangledSource::getProperty(const PropertyId& property_id) const {

  // If we already have the property stored in this object, returns it
  if (m_property_holder.isPropertySet(property_id)) {
    return m_property_holder.getProperty(property_id);
  }

  // Gets a pointer to the group we belong to
  auto group = m_group.lock();
  if (!group) {
    // Something went wrong, we don't have a pointer to the group, this should not happen
    // FIXME use a more specific exception
    throw PropertyNotFoundException();
  }

  // If the property is already stored in the group, we return it
  if (group->m_property_holder.isPropertySet(property_id)) {
    return group->getProperty(property_id);
  }

  try {
    // Try to get the the property from the encapsulated Source
    // if it cannot provide it, this will throw a PropertyNotFoundException
    return m_source->getProperty(property_id);
  } catch (PropertyNotFoundException& e) {
    // Getting this exception means the property must be computed at the group level

    // Get the group task
    auto group_task = m_task_registry->getTask<GroupTask>(property_id);
    if (!group_task) {
      // No task is available to make that property
      throw PropertyNotFoundException();
    }

    // Use the task to make the property
    group_task->computeProperties(*group);

    // The property should now be available either in this object or in the group object
    if (m_property_holder.isPropertySet(property_id)) {
      return m_property_holder.getProperty(property_id);
    } else {
      return group->m_property_holder.getProperty(property_id);
    }
  }
}

void EntangledSourceGroup::EntangledSource::setProperty(
    std::unique_ptr<Property> property, const PropertyId& property_id) {
  m_property_holder.setProperty(std::move(property), property_id);
}

/*********************************************************************************************************************/

const Property& EntangledSourceGroup::EntangledSourceWrapper::getProperty(const PropertyId& property_id) const  {
  // Just forwards the call
  return m_source->getProperty(property_id);
}

void EntangledSourceGroup::EntangledSourceWrapper::setProperty(
    std::unique_ptr<Property> property, const PropertyId& property_id)  {
  // Just forwards the call
  m_source->setProperty(std::move(property), property_id);
}


} // SEFramework namespace


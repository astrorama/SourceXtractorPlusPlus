/**
 * @file src/lib/EntangledSourceGroup.cpp
 * @date 05/09/16
 * @author mschefer
 */

#include "DataModel/TaskRegistry.h"
#include "DataModel/GroupTask.h"
#include "DataModel/Source.h"
#include "DataModel/PropertyNotFoundException.h"

#include "DataModel/EntangledSourceGroup.h"

namespace DataModel {

EntangledSourceGroup::EntangledSourceGroup(std::shared_ptr<TaskRegistry> task_registry)
    : m_task_registry(task_registry) {}

void EntangledSourceGroup::setSources(std::vector<std::shared_ptr<Source>> sources) {
  m_sources.reserve(sources.size());
  for (auto& source : sources) {
    // Encapsulates every Source into an EntangledSource object and stores it
    m_sources.push_back(std::make_shared<EntangledSource>(source, shared_from_this(), m_task_registry));
  }
}

const std::vector<std::shared_ptr<SourceInterface>> EntangledSourceGroup::getSources() const {
  std::vector<std::shared_ptr<SourceInterface>> sources;
  sources.reserve(m_sources.size());
  for (auto source : m_sources) {
    // Encapsulates every EntangledSource into an EntangledSourceWrapper before returning it
    sources.push_back(std::make_shared<EntangledSourceWrapper>(source, shared_from_this()));
  }
  return sources;
}

const std::vector<std::shared_ptr<ModifiableSource>> EntangledSourceGroup::getModifiableSources() {
  std::vector<std::shared_ptr<ModifiableSource>> sources;
  sources.reserve(m_sources.size());
  for (auto source : m_sources) {
    // Encapsulates every EntangledSource into an EntangledSourceWrapper before returning it
    sources.push_back(std::make_shared<EntangledSourceWrapper>(source, shared_from_this()));
  }
  return sources;
}

Property& EntangledSourceGroup::getPropertyImpl(const PropertyId property_id) const {
  // If we already have the property, return it
  if (isPropertySet(property_id)) {
    return ObjectWithProperties::getPropertyImpl(property_id);
  }

  // If not, get the task for that property, use it to compute the property then return it
  auto task = m_task_registry->getTask<GroupTask>(property_id);
  if (task) {
    task->computeProperties(const_cast<EntangledSourceGroup&>(*this));
    return ObjectWithProperties::getPropertyImpl(property_id);
  }

  // No task available to make that property, we throw an exception
  throw PropertyNotFoundException();
}

void EntangledSourceGroup::setPropertyImpl(std::unique_ptr<Property> property, PropertyId property_id) {
  ObjectWithProperties::setPropertyImpl(std::move(property), property_id);
}

/*********************************************************************************************************************/

EntangledSourceGroup::EntangledSource::EntangledSource(std::shared_ptr<Source> source,
    std::shared_ptr<EntangledSourceGroup> group, std::shared_ptr<TaskRegistry> task_registry)
      : m_source(source), m_group(group), m_task_registry(task_registry) {}

Property& EntangledSourceGroup::EntangledSource::getPropertyImpl(const PropertyId property_id) const {

  // If we already have the property stored in this object, returns it
  if (isPropertySet(property_id)) {
    return ObjectWithProperties::getPropertyImpl(property_id);
  }

  // Gets a pointer to the group we belong to
  auto group = m_group.lock();
  if (!group) {
    // Something went wrong, we don't have a pointer to the group, this should not happen
    // FIXME use a more specific exception
    throw PropertyNotFoundException();
  }

  // If the property is already stored in the group, we return it
  if (group->isPropertySet(property_id)) {
    return group->getPropertyImpl(property_id);
  }

  try {
    // Try to get the the property from the encapsulated Source
    // if it cannot provide it, this will throw a PropertyNotFoundException
    return m_source->getPropertyImpl(property_id);
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
    if (isPropertySet(property_id)) {
      return ObjectWithProperties::getPropertyImpl(property_id);
    } else {
      return group->ObjectWithProperties::getPropertyImpl(property_id);
    }
  }
}

void EntangledSourceGroup::EntangledSource::setPropertyImpl(
    std::unique_ptr<Property> property, PropertyId property_id) {
  ObjectWithProperties::setPropertyImpl(std::move(property), property_id);
}

/*********************************************************************************************************************/

Property& EntangledSourceGroup::EntangledSourceWrapper::getPropertyImpl(
    const PropertyId property_id) const  {
  // Just forwards the call
  return m_source->getPropertyImpl(property_id);
}

void EntangledSourceGroup::EntangledSourceWrapper::setPropertyImpl(std::unique_ptr<Property> property,
    PropertyId property_id)  {
  // Just forwards the call
  m_source->setPropertyImpl(std::move(property), property_id);
}


} // DataModel namespace


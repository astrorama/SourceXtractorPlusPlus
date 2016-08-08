/**
 * @file src/lib/Source/SourceGroup.cpp
 * @date 08/04/16
 * @author nikoapos
 */

#include "SEFramework/Source/SourceGroup.h"
#include "SEFramework/Task/GroupTask.h"

namespace SExtractor {

SourceGroup::SourceGroup(std::shared_ptr<TaskProvider> task_provider)
        : m_task_provider(task_provider) {
}

SourceGroup::iterator SourceGroup::begin() {
  return iterator(m_sources.begin());
}

SourceGroup::iterator SourceGroup::end() {
  return iterator(m_sources.end());
}

SourceGroup::const_iterator SourceGroup::cbegin() {
  return const_iterator(m_sources.begin());
}

SourceGroup::const_iterator SourceGroup::cend() {
  return const_iterator(m_sources.end());
}

SourceGroup::const_iterator SourceGroup::begin() const {
  return const_iterator(m_sources.begin());
}

SourceGroup::const_iterator SourceGroup::end() const {
  return const_iterator(m_sources.end());
}

void SourceGroup::addSource(std::shared_ptr<SourceInterface> source) {
  clearGroupProperties();
  m_sources.emplace(source, *this);
}

SourceGroup::iterator SourceGroup::removeSource(iterator pos) {
  clearGroupProperties();
  return iterator(m_sources.erase(pos.m_entangled_it));
  
}

void SourceGroup::merge(const SourceGroup& other) {
  // We go through the EntangledSources of the other group and we create new ones
  // locally, pointing to the same wrapped sources. This is necessary, so the
  // new EntangledSources have a reference to the correct group.
  for (auto& source : other.m_sources) {
    this->m_sources.emplace( source.m_source, *this);
  }
  this->clearGroupProperties();
}

const Property& SourceGroup::getProperty(const PropertyId& property_id) const {
  // If we already have the property, return it
  if (m_property_holder.isPropertySet(property_id)) {
    return m_property_holder.getProperty(property_id);
  }

  // If not, get the task for that property, use it to compute the property then return it
  auto task = m_task_provider->getTask<GroupTask>(property_id);
  if (task) {
    task->computeProperties(const_cast<SourceGroup&>(*this));
    return m_property_holder.getProperty(property_id);
  }

  // No task available to make that property, we throw an exception
  throw PropertyNotFoundException();
}

void SourceGroup::setProperty(std::unique_ptr<Property> property, const PropertyId& property_id) {
  m_property_holder.setProperty(std::move(property), property_id);
}

void SourceGroup::clearGroupProperties() {
  m_property_holder.clear();
  for (auto& source : m_sources) {
    const_cast<EntangledSource&>(source).m_property_holder.clear();
  }
}










} // SExtractor namespace




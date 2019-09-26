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
 * @file src/lib/Source/SourceGroup.cpp
 * @date 08/04/16
 * @author nikoapos
 */

#include "SEFramework/Source/SourceGroupWithOnDemandProperties.h"
#include "SEFramework/Task/GroupTask.h"

namespace SExtractor {

SourceGroupWithOnDemandProperties::SourceGroupWithOnDemandProperties(std::shared_ptr<TaskProvider> task_provider)
        : m_task_provider(task_provider) {
}

SourceGroupWithOnDemandProperties::iterator SourceGroupWithOnDemandProperties::begin() {
  return iterator(std::unique_ptr<IteratorImpl>(new iter{m_sources.begin()}));
}

SourceGroupWithOnDemandProperties::iterator SourceGroupWithOnDemandProperties::end() {
  return iterator(std::unique_ptr<IteratorImpl>(new iter{m_sources.end()}));
}

SourceGroupWithOnDemandProperties::const_iterator SourceGroupWithOnDemandProperties::cbegin() {
  return const_iterator(std::unique_ptr<IteratorImpl>(new iter{m_sources.begin()}));
}

SourceGroupWithOnDemandProperties::const_iterator SourceGroupWithOnDemandProperties::cend() {
  return const_iterator(std::unique_ptr<IteratorImpl>(new iter{m_sources.end()}));
}

SourceGroupWithOnDemandProperties::const_iterator SourceGroupWithOnDemandProperties::begin() const {
  return const_iterator(std::unique_ptr<IteratorImpl>(new iter{const_cast<SourceGroupWithOnDemandProperties*>(this)->m_sources.begin()}));
}

SourceGroupWithOnDemandProperties::const_iterator SourceGroupWithOnDemandProperties::end() const {
  return const_iterator(std::unique_ptr<IteratorImpl>(new iter{const_cast<SourceGroupWithOnDemandProperties*>(this)->m_sources.end()}));
}

void SourceGroupWithOnDemandProperties::addSource(std::shared_ptr<SourceInterface> source) {
  clearGroupProperties();
  m_sources.emplace_back(source, *this);
}

SourceGroupWithOnDemandProperties::iterator SourceGroupWithOnDemandProperties::removeSource(iterator pos) {
  iter& iter_impl = dynamic_cast<iter&>(pos.getImpl());
  auto next_entangled_it = m_sources.erase(iter_impl.m_entangled_it);
  clearGroupProperties();
  return iterator(std::unique_ptr<IteratorImpl>(new iter{next_entangled_it}));
}

void SourceGroupWithOnDemandProperties::merge(const SourceGroupInterface& other) {
  auto& other_group = dynamic_cast<const SourceGroupWithOnDemandProperties&>(other);
  // We go through the EntangledSources of the other group and we create new ones
  // locally, pointing to the same wrapped sources. This is necessary, so the
  // new EntangledSources have a reference to the correct group.
  for (auto& source : other_group.m_sources) {
    this->m_sources.emplace_back( source.m_source, *this);
  }
  this->clearGroupProperties();
}

const Property& SourceGroupWithOnDemandProperties::getProperty(const PropertyId& property_id) const {
  // If we already have the property, return it
  if (m_property_holder.isPropertySet(property_id)) {
    return m_property_holder.getProperty(property_id);
  }

  // If not, get the task for that property, use it to compute the property then return it
  auto task = m_task_provider->getTask<GroupTask>(property_id);
  if (task) {
    task->computeProperties(const_cast<SourceGroupWithOnDemandProperties&>(*this));
    return m_property_holder.getProperty(property_id);
  }

  // No task available to make that property, we throw an exception
  throw PropertyNotFoundException(property_id);
}

void SourceGroupWithOnDemandProperties::setProperty(std::unique_ptr<Property> property, const PropertyId& property_id) {
  m_property_holder.setProperty(std::move(property), property_id);
}

void SourceGroupWithOnDemandProperties::clearGroupProperties() {
  m_property_holder.clear();
  for (auto& source : m_sources) {
    const_cast<EntangledSource&>(source).m_property_holder.clear();
  }
}

unsigned int SourceGroupWithOnDemandProperties::size() const {
  return m_sources.size();
}

} // SExtractor namespace




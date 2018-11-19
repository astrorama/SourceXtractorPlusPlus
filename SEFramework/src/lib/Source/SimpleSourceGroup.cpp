/* 
 * @file SimpleSourceGroup.cpp
 * @author nikoapos
 */

#include "SEFramework/Source/SimpleSourceGroup.h"

namespace SExtractor {

SimpleSourceGroup::iterator SimpleSourceGroup::begin() {
  return iterator(std::unique_ptr<IteratorImpl>(new iter{m_sources.begin()}));
}

SimpleSourceGroup::iterator SimpleSourceGroup::end() {
  return iterator(std::unique_ptr<IteratorImpl>(new iter{m_sources.end()}));
}

SimpleSourceGroup::const_iterator SimpleSourceGroup::cbegin() {
  return const_iterator(std::unique_ptr<IteratorImpl>(new iter{m_sources.begin()}));
}

SimpleSourceGroup::const_iterator SimpleSourceGroup::cend() {
  return const_iterator(std::unique_ptr<IteratorImpl>(new iter{m_sources.end()}));
}

SimpleSourceGroup::const_iterator SimpleSourceGroup::begin() const {
  return const_iterator(std::unique_ptr<IteratorImpl>(new iter{const_cast<SimpleSourceGroup*>(this)->m_sources.begin()}));
}

SimpleSourceGroup::const_iterator SimpleSourceGroup::end() const {
  return const_iterator(std::unique_ptr<IteratorImpl>(new iter{const_cast<SimpleSourceGroup*>(this)->m_sources.end()}));
}

void SimpleSourceGroup::addSource(std::shared_ptr<SourceInterface> source) {
  m_sources.push_back(source);
}

SourceGroupInterface::iterator SimpleSourceGroup::removeSource(iterator pos) {
  iter& iter_impl = dynamic_cast<iter&>(pos.getImpl());
  auto next_wrapped_it = m_sources.erase(iter_impl.m_wrapped_it);
  return iterator(std::unique_ptr<IteratorImpl>(new iter{next_wrapped_it}));
}

void SimpleSourceGroup::merge(const SourceGroupInterface& other) {
  auto& other_group = dynamic_cast<const SimpleSourceGroup&>(other);
  addAllSources(other_group.m_sources);
}

const Property& SimpleSourceGroup::getProperty(const PropertyId& property_id) const {
  return m_property_holder.getProperty(property_id);
}

void SimpleSourceGroup::setProperty(std::unique_ptr<Property> property, const PropertyId& property_id) {
  m_property_holder.setProperty(std::move(property), property_id);
}

} // SExtractor namespace

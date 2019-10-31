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
/* 
 * @file SimpleSourceGroup.h
 * @author nikoapos
 */

#ifndef _SEFRAMEWORK_SIMPLESOURCEGROUP_H
#define _SEFRAMEWORK_SIMPLESOURCEGROUP_H

#include <list>

#include "SEFramework/Source/SourceGroupInterface.h"
#include "SEFramework/Property/PropertyHolder.h"

namespace SourceXtractor {

/**
 * @class SimpleSourceGroup
 * @brief A bare bones implementation of SourceGroupInterface meant to be used to implement unit tests
 *
 */


class SimpleSourceGroup : public SourceGroupInterface {
  
public:
  
  virtual ~SimpleSourceGroup() = default;

  iterator begin() override;
  
  iterator end() override;
  
  const_iterator cbegin() override;
  
  const_iterator cend() override;
  
  const_iterator begin() const override;
  
  const_iterator end() const override;
  
  void addSource(std::shared_ptr<SourceInterface> source) override;
  
  iterator removeSource(iterator pos) override;
  
  unsigned int size() const override;

  void merge(const SourceGroupInterface& other) override;
  
  using SourceInterface::getProperty;
  using SourceInterface::setProperty;

protected:
  
  const Property& getProperty(const PropertyId& property_id) const override;

  void setProperty(std::unique_ptr<Property> property, const PropertyId& property_id) override;
  
private:
  
  class iter;
  std::list<std::shared_ptr<SourceInterface>> m_sources;
  PropertyHolder m_property_holder;
  
}; /* End of SimpleSourceGroup class */


class SimpleSourceGroup::iter : public SourceGroupInterface::IteratorImpl {
  
public:
  iter(std::list<std::shared_ptr<SourceInterface>>::iterator wrapped_it)
          : m_wrapped_it(wrapped_it) {
  }

  virtual ~iter() = default;
  
  SourceInterface& dereference() const override {
    return const_cast<SourceInterface&>(**m_wrapped_it);
  }
  
  void increment() override {
    ++m_wrapped_it;
  }
  
  void decrement() override {
    --m_wrapped_it;
  }
  
  bool equal(const IteratorImpl& other) const override {
      try {
        auto& other_iter = dynamic_cast<const iter&>(other);
        return this->m_wrapped_it == other_iter.m_wrapped_it;
      } catch (...) {
        return false;
      }
  }

  std::shared_ptr<IteratorImpl> clone() const override {
    return std::make_shared<iter>(m_wrapped_it);
  }


private:
  
  std::list<std::shared_ptr<SourceInterface>>::iterator m_wrapped_it;
  
  friend SimpleSourceGroup::iterator SimpleSourceGroup::removeSource(SimpleSourceGroup::iterator);
  
};

} /* namespace SourceXtractor */

#endif /* _SEFRAMEWORK_SIMPLESOURCEGROUP_H */


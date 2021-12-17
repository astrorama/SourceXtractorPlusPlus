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
 * @file SourceGroupInterface.h
 * @author nikoapos
 */

#ifndef _SEFRAMEWORK_SOURCEGROUPINTERFACE_H
#define _SEFRAMEWORK_SOURCEGROUPINTERFACE_H

#include "SEFramework/Source/SourceInterface.h"

namespace SourceXtractor {

/**
 * @class SourceGroupInterface
 * @brief Defines the interface used to group sources
 *
 * @details Warning: please note that the order in which the sources are added to a group is not preserved.
 *
 */

class SourceGroupInterface : protected SourceInterface {
  
  template <typename Collection>
  using CollectionType = typename std::iterator_traits<typename Collection::iterator>::value_type;

  // This is used to determine if a type is a kind of std::shared_ptr
  template <class T>
  struct is_shared_ptr : std::false_type {};
  template <class T>
  struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

public:
  
  class SourceWrapper : public SourceInterface {
  public:

    explicit SourceWrapper(std::shared_ptr<SourceInterface> source) : m_source(source) {}

    SourceWrapper(const SourceWrapper& source) : m_source(source.m_source) {}

    const Property& getProperty(const PropertyId& property_id) const override {
      return m_source->getProperty(property_id);
    }

    void setProperty(std::unique_ptr<Property> property, const PropertyId& property_id) override {
      m_source->setProperty(std::move(property), property_id);
    }

    bool operator<(const SourceWrapper& other) const {
      return this->m_source < other.m_source;
    }

    SourceInterface& getRef() const {
      return *m_source;
    }

    using SourceInterface::getProperty;
    using SourceInterface::setProperty;
    using SourceInterface::setIndexedProperty;

  private:
    std::shared_ptr<SourceInterface> m_source;
  };

  using iterator = std::list<SourceWrapper>::iterator;
  using const_iterator = std::list<SourceWrapper>::const_iterator;

  virtual iterator begin() = 0;
  virtual iterator end() = 0;
  virtual const_iterator cbegin() = 0;
  virtual const_iterator cend() = 0;
  virtual const_iterator begin() const = 0;
  virtual const_iterator end() const = 0;
  
  virtual void addSource(std::shared_ptr<SourceInterface> source) = 0;
  virtual iterator removeSource(iterator pos) = 0;
  virtual void merge(const SourceGroupInterface& other) = 0;
  virtual unsigned int size() const = 0;
  
  /// Convenient method to add all the sources of a collection
  template <typename SourceCollection>
  void addAllSources(const SourceCollection& sources) {
    static_assert(is_shared_ptr<CollectionType<SourceCollection>>::value,
        "SourceCollection must be a collection of std::shared_ptr");
    static_assert(std::is_base_of<SourceInterface, typename CollectionType<SourceCollection>::element_type>::value,
        "SourceCollection must be a collection of std::shared_ptr to SourceInterface or a type that inherits from it");
    for (auto& source : sources) {
      addSource(source);
    }
  }
  
  // We introduce the get/setProperty methods from the SourceInterface in the
  // public symbols so they become part of the SourceGroupInterface. The group
  // implementations must implement the methods with the PropertyId
  // in their signature.
  using SourceInterface::getProperty;
  using SourceInterface::setProperty;
  using SourceInterface::setIndexedProperty;
  
}; // end of SourceGroupInterface class 

} /* namespace SourceXtractor */

#endif /* _SEFRAMEWORK_SOURCEGROUPINTERFACE_H */


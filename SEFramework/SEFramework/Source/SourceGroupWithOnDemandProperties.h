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
 * @file SEFramework/Source/SourceGroupWithOnDemandProperties.h
 * @date 08/04/16
 * @author nikoapos
 */

#ifndef _SEFRAMEWORK_SOURCE_SOURCEGROUP_H
#define _SEFRAMEWORK_SOURCE_SOURCEGROUP_H

#include <set>
#include <iterator>
#include <type_traits>

#include "SEFramework/Source/SourceInterface.h"
#include "SEFramework/Task/TaskProvider.h"
#include "SEFramework/Property/PropertyHolder.h"

namespace SourceXtractor {

/**
 * @class SourceGroupWithOnDemandProperties
 * @brief A SourceGroupInterface implementation which used a TaskProvider to compute missing properties
 *
 */
class SourceGroupWithOnDemandProperties: public SourceInterface {
protected:
  class EntangledSource;

  template <typename Collection>
  using CollectionType = typename std::iterator_traits<typename Collection::iterator>::value_type;

  // This is used to determine if a type is a kind of std::shared_ptr
  template <class T>
  struct is_shared_ptr : std::false_type {};
  template <class T>
  struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

public:

  typedef std::list<EntangledSource>::iterator iterator;
  typedef std::list<EntangledSource>::const_iterator const_iterator;
  
  SourceGroupWithOnDemandProperties(std::shared_ptr<TaskProvider> task_provider);

  /**
   * @brief Destructor
   */
  virtual ~SourceGroupWithOnDemandProperties() = default;

  iterator begin();
  
  iterator end();
  
  const_iterator cbegin();
  
  const_iterator cend();
  
  const_iterator begin() const;
  
  const_iterator end() const;
  
  void addSource(std::shared_ptr<SourceInterface> source);

  template<typename SourceCollection>
  void addAllSources(const SourceCollection& sources) {
    static_assert(is_shared_ptr < CollectionType < SourceCollection >> ::value,
                  "SourceCollection must be a collection of std::shared_ptr");
    static_assert(std::is_base_of<SourceInterface, typename CollectionType<SourceCollection>::element_type>::value,
                  "SourceCollection must be a collection of std::shared_ptr to SourceInterface or a type that inherits from it");
    for (auto& source : sources) {
      addSource(source);
    }
  }


  iterator removeSource(iterator pos);
  
  void merge(const SourceGroupWithOnDemandProperties& other);
  
  unsigned int size() const;

  // Note : Because the get/setProperty() methods of the SourceInterface are
  // templated, the overrides of the non-templated versions will hide them. For
  // this reason it is necessary to re-introduce the templated methods, which is
  // done by the using statements below.
  using SourceInterface::getProperty;
  using SourceInterface::setProperty;

protected:
  class EntangledSource;

  const Property& getProperty(const PropertyId& property_id) const final;

  void setProperty(std::unique_ptr<Property> property, const PropertyId& property_id) final;

private:

  std::list<EntangledSource> m_sources;
  PropertyHolder m_property_holder;
  std::shared_ptr<TaskProvider> m_task_provider;
  
  void clearGroupProperties();

}; /* End of SourceGroup class */



class SourceGroupWithOnDemandProperties::EntangledSource : public SourceInterface {
  
public:
  
  EntangledSource(std::shared_ptr<SourceInterface> source, SourceGroupWithOnDemandProperties& group);

  virtual ~EntangledSource() = default;

  bool operator<(const EntangledSource& other) const;

  // Note : Because the get/setProperty() methods of the SourceInterface are
  // templated, the overrides of the non-templated versions will hide them. For
  // this reason it is necessary to re-introduce the templated methods, which is
  // done by the using statements below.
  using SourceInterface::getProperty;
  using SourceInterface::setProperty;

private:
  
  PropertyHolder m_property_holder;
  std::shared_ptr<SourceInterface> m_source;
  SourceGroupWithOnDemandProperties& m_group;

  const Property& getProperty(const PropertyId& property_id) const override;
  void setProperty(std::unique_ptr<Property> property, const PropertyId& property_id) override;

  friend void SourceGroupWithOnDemandProperties::clearGroupProperties();
  friend void SourceGroupWithOnDemandProperties::merge(const SourceGroupWithOnDemandProperties&);
  
};

} /* namespace SourceXtractor */

#endif

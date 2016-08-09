/**
 * @file SEFramework/Source/SourceGroup.h
 * @date 08/04/16
 * @author nikoapos
 */

#ifndef _SEFRAMEWORK_SOURCE_SOURCEGROUP_H
#define _SEFRAMEWORK_SOURCE_SOURCEGROUP_H

#include <set>
#include <iterator>
#include <type_traits>

#include "SEFramework/Source/SourceInterface.h"
#include "SEFramework/Source/SourceGroupInterface.h"
#include "SEFramework/Task/TaskProvider.h"
#include "SEFramework/Property/PropertyHolder.h"

namespace SExtractor {

/**
 * @class SourceGroup
 * @brief
 *
 */
class SourceGroup : public SourceGroupInterface {
  
  template <typename Collection>
  using CollectionType = typename std::iterator_traits<typename Collection::iterator>::value_type;
  
  template <typename T>
  using EnableIfSourcePtr = typename std::enable_if<std::is_same<CollectionType<T>, std::shared_ptr<SourceInterface>>::value>;

public:
  
  SourceGroup(std::shared_ptr<TaskProvider> task_provider);
  
  template <typename SourceCollection, typename EnableIfSourcePtr<SourceCollection>::type* = nullptr>
  SourceGroup(std::shared_ptr<TaskProvider> task_provider, const SourceCollection& sources)
          : SourceGroup(task_provider) {
    for (auto& source : sources) {
      m_sources.emplace(source, *this);
    }
  }

  /**
   * @brief Destructor
   */
  virtual ~SourceGroup() = default;
  
  iterator begin();
  
  iterator end();
  
  const_iterator cbegin();
  
  const_iterator cend();
  
  const_iterator begin() const;
  
  const_iterator end() const;
  
  void addSource(std::shared_ptr<SourceInterface> source);
  
  iterator removeSource(iterator pos);
  
  void merge(const SourceGroupInterface& other);
  
  using SourceInterface::getProperty;
  using SourceInterface::setProperty;

protected:
  
  const Property& getProperty(const PropertyId& property_id) const override;

  void setProperty(std::unique_ptr<Property> property, const PropertyId& property_id) override;

private:
  
  class iter;
  class EntangledSource;
  std::set<EntangledSource> m_sources;
  PropertyHolder m_property_holder;
  std::shared_ptr<TaskProvider> m_task_provider;
  
  void clearGroupProperties();

}; /* End of SourceGroup class */



class SourceGroup::EntangledSource : public SourceInterface {
  
public:
  
  EntangledSource(std::shared_ptr<SourceInterface> source, SourceGroup& group);

  virtual ~EntangledSource() = default;

  const Property& getProperty(const PropertyId& property_id) const override;

  void setProperty(std::unique_ptr<Property> property, const PropertyId& property_id) override;
  
  bool operator<(const EntangledSource& other) const;

private:
  
  PropertyHolder m_property_holder;
  std::shared_ptr<SourceInterface> m_source;
  SourceGroup& m_group;
  
  friend void SourceGroup::clearGroupProperties();
  friend void SourceGroup::merge(const SourceGroupInterface&);
  
};


class SourceGroup::iter : public SourceGroup::IteratorImpl {
  
public:
  
  iter(std::set<EntangledSource>::iterator m_entangled_it) 
          : m_entangled_it(m_entangled_it) {
  }

  virtual ~iter() = default;
  
  // Note to developers
  // The std::set provides only constant iterator, because modifying its entries
  // might mean that their ordering (which is used internally) might change. In
  // our case we have no such problem, because the ordering of the EntangledSource
  // is based on the pointer address of the encapsulated Source. This allows
  // for the following const casts, so if the user iterates over a non-const
  // SourceGroup he will get Sources on which he can call the setProperty().
  SourceInterface& dereference() const override {
    return const_cast<EntangledSource&>(*m_entangled_it);
  }
  
  void increment() override {
    ++m_entangled_it;
  }
  
  void decrement() override {
    --m_entangled_it;
  }
  
  bool equal(const IteratorImpl& other) const override {
      try {
        auto& other_iter = dynamic_cast<const iter&>(other);
        return this->m_entangled_it == other_iter.m_entangled_it;
      } catch (...) {
        return false;
      }
  }

private:
  
  std::set<EntangledSource>::iterator m_entangled_it;
  
  friend SourceGroup::iterator SourceGroup::removeSource(SourceGroup::iterator);
  
};

} /* namespace SExtractor */

#endif

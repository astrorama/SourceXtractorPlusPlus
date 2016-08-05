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

#include "SEFramework/Source/Source.h"
#include "SEFramework/Source/SourceInterface.h"
#include "SEFramework/Task/TaskProvider.h"

namespace SExtractor {

/**
 * @class SourceGroup
 * @brief
 *
 */
class SourceGroup : public SourceInterface {
  
  template <typename Collection>
  using CollectionType = typename std::iterator_traits<typename Collection::iterator>::value_type;
  
  template <typename T>
  using EnableIfSourcePtr = typename std::enable_if<std::is_same<CollectionType<T>, std::shared_ptr<Source>>::value>;

public:
  
  template <typename T>
  class iter;
  using iterator = iter<SourceInterface>;
  using const_iterator = iter<const SourceInterface>;
  
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
  
  void addSource(std::shared_ptr<Source> source);
  
  iterator removeSource(iterator pos);

protected:
  
  const Property& getProperty(const PropertyId& property_id) const override;

  void setProperty(std::unique_ptr<Property> property, const PropertyId& property_id) override;

private:
  
  class EntangledSource;
  std::set<EntangledSource> m_sources;
  PropertyHolder m_property_holder;
  std::shared_ptr<TaskProvider> m_task_provider;

}; /* End of SourceGroup class */



struct ToDoReplaceWithGroupTask : Task {
  void computeProperties(SourceGroup&) const {
    
  }
};

class SourceGroup::EntangledSource : public SourceInterface {
  
public:
  
  EntangledSource(std::shared_ptr<Source> source, SourceGroup& group)
          : m_source(source), m_group(group) {
  }

  virtual ~EntangledSource() = default;

  const Property& getProperty(const PropertyId& property_id) const override {

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
      return m_source->getProperty(property_id);
    } catch (PropertyNotFoundException& e) {
      // Getting this exception means the property must be computed at the group level

      // Get the group task
      auto group_task = m_group.m_task_provider->getTask<ToDoReplaceWithGroupTask>(property_id);
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

  void setProperty(std::unique_ptr<Property> property, const PropertyId& property_id) override {
    m_property_holder.setProperty(std::move(property), property_id);
  }
  
  bool operator<(const EntangledSource& other) const {
    return this->m_source < other.m_source;
  }

private:
  
  PropertyHolder m_property_holder;
  std::shared_ptr<Source> m_source;
  SourceGroup& m_group;
  
};


template <typename T>
class SourceGroup::iter : public std::iterator<std::forward_iterator_tag, T> {
  
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
  
  T& operator*() const {
    return const_cast<EntangledSource&>(*m_entangled_it);
  }
  
  T* operator->() const {
    return const_cast<EntangledSource*>(&(*m_entangled_it));
  }
  
  iter& operator++() {
    ++m_entangled_it;
    return *this;
  }
  
  bool operator==(const iter& other) const {
    return this->m_entangled_it == other.m_entangled_it;
  }
  
  bool operator!=(const iter& other) const {
    return this->m_entangled_it != other.m_entangled_it;
  }

private:
  
  std::set<EntangledSource>::iterator m_entangled_it;
  
  friend SourceGroup::iterator SourceGroup::removeSource(SourceGroup::iterator);
  
};

} /* namespace SExtractor */

#endif

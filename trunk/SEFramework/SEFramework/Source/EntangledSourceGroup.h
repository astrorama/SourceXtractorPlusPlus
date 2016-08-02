/**
 * @file SEFramework/Source/EntangledSourceGroup.h
 * @date 05/09/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_SOURCE_ENTANGLEDSOURCEGROUP_H
#define _SEFRAMEWORK_SOURCE_ENTANGLEDSOURCEGROUP_H

#include <vector>
#include <list>
#include <functional>

#include "SEFramework/Property/PropertyHolder.h"
#include "SEFramework/Source/SourceInterface.h"

namespace SExtractor {

class TaskRegistry;
class Source;

/**
 * @class EntangledSourceGroup
 * @brief Represents a group of Sources that must be processed together
 *
 */
class EntangledSourceGroup : public std::enable_shared_from_this<EntangledSourceGroup> {

private:
  class EntangledSourceWrapper;

  /**
   * @class EntangledSource
   * @brief A source inside an EntangledSourceGroup. Encapsulates Source and adds group related properties to it.
   */

  class EntangledSource : public SourceInterface {
  public:

    virtual ~EntangledSource() = default;

    EntangledSource(std::shared_ptr<Source> source, std::shared_ptr<EntangledSourceGroup> group,
        std::shared_ptr<TaskRegistry> task_registry);

    // Implementation of SourceInterface
    virtual const Property& getProperty(const PropertyId& property_id) const override;
    virtual void setProperty(std::unique_ptr<Property> property, const PropertyId& property_id) override;

  private:
    PropertyHolder m_property_holder;

    std::shared_ptr<Source> m_source;
    std::weak_ptr<EntangledSourceGroup> m_group;
    std::shared_ptr<TaskRegistry> m_task_registry;
  };

  /**
   * @class EntangledSourceWrapper
   * @brief A simple wrapper around EntangledSource used for memory management.
   *
   * @details In order to keep both EntangledSources and their EntangledSourceGroup in memory without
   * having a circular dependency of shared_ptrs, we always return EntangledSources encapsulated within a
   * EntangledSourceWrapper that contains both a shared_ptr to the source and to the group.
   */

  class EntangledSourceWrapper : public SourceInterface {
  public:
    EntangledSourceWrapper(std::shared_ptr<EntangledSource> source, std::shared_ptr<const EntangledSourceGroup> group)
      : m_source(source), m_group(group) {}
      
  protected:
    // Implementation of SourceInterface (just simple forward to the encapsulated EntangledSource
    virtual const Property& getProperty(const PropertyId& property_id) const override;
    virtual void setProperty(std::unique_ptr<Property> property, const PropertyId& property_id) override;

  private:
    std::shared_ptr<EntangledSource> m_source;
    std::shared_ptr<const EntangledSourceGroup> m_group;
  };

public:

  /// Factory method to create an EntangledSourceGroup. For implementation reasons,
  /// the constructor must be private and this method must be used instead.

  static std::shared_ptr<EntangledSourceGroup> create(std::list<std::shared_ptr<Source>> sources,
      std::shared_ptr<TaskRegistry> task_registry) {
    auto new_instance = std::shared_ptr<EntangledSourceGroup>(new EntangledSourceGroup(task_registry));
    new_instance->setSources(sources);
    return new_instance;
  }

  /**
   * @brief Destructor
   */
  virtual ~EntangledSourceGroup() = default;

  /// returns a vector of pointers to the sources contained in this group as const SourceInterface
  virtual std::vector<std::shared_ptr<const SourceInterface>> getSources() const;

  /// returns a vector of pointers to the sources contained in this group as SourceInterfaces
  virtual std::vector<std::shared_ptr<SourceInterface>> getSources();

  // implementation actual implementation used by the getProperty() and setProperty() methods
  virtual const Property& getProperty(const PropertyId& property_id) const;
  virtual void setProperty(std::unique_ptr<Property> property, const PropertyId& property_id);

  /// Convenience template method to call getProperty() with a more user-friendly syntax
  template<typename T>
  const T& getProperty(unsigned int index = 0) const {
    return dynamic_cast<const T&>(getProperty(PropertyId::create<T>(index)));
  }

  /// Convenience template method to call setProperty() with a more user-friendly syntax
  template<typename T>
  void setProperty(std::unique_ptr<T> property, unsigned int index = 0) {
    setProperty(std::move(property), PropertyId::create<T>(index));
  }

private:
  PropertyHolder m_property_holder;

  EntangledSourceGroup(std::shared_ptr<TaskRegistry> task_registry);
  void setSources(std::list<std::shared_ptr<Source>> sources);

  std::vector<std::shared_ptr<EntangledSource>> m_sources;
  std::shared_ptr<TaskRegistry> m_task_registry;
}; /* End of EntangledSourceGroup class */

} /* namespace SExtractor */


#endif

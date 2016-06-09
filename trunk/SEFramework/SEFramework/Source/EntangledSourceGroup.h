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

#include "SEFramework/Property/ObjectWithProperties.h"
#include "ModifiableSource.h"

namespace SEFramework {

class TaskRegistry;
class Source;

/**
 * @class EntangledSourceGroup
 * @brief Represents a group of Sources that must be processed together
 *
 */
class EntangledSourceGroup : protected ObjectWithProperties, public std::enable_shared_from_this<EntangledSourceGroup> {

private:
  class EntangledSourceWrapper;

  /**
   * @class EntangledSource
   * @brief A source inside an EntangledSourceGroup. Encapsulates Source and adds group related properties to it.
   */

  class EntangledSource : public ModifiableSource, protected ObjectWithProperties  {
  public:

    virtual ~EntangledSource() = default;

    EntangledSource(std::shared_ptr<Source> source, std::shared_ptr<EntangledSourceGroup> group,
        std::shared_ptr<TaskRegistry> task_registry);

    // Implementation of ModifiableSource
    virtual Property& getPropertyImpl(const PropertyId property_id) const override;
    virtual void setPropertyImpl(std::unique_ptr<Property> property, PropertyId property_id) override;

  private:
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

  class EntangledSourceWrapper : public ModifiableSource {
  public:
    EntangledSourceWrapper(std::shared_ptr<EntangledSource> source, std::shared_ptr<const EntangledSourceGroup> group)
      : m_source(source), m_group(group) {}

    // Implementation of ModifiableSource (just simple forward to the encapsulated EntangledSource
    virtual Property& getPropertyImpl(const PropertyId property_id) const override;
    virtual void setPropertyImpl(std::unique_ptr<Property> property, PropertyId property_id) override;

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

  /// returns a vector of pointers to the sources contained in this group as SourceInterface
  virtual std::vector<std::shared_ptr<const SourceInterface>> getSources() const;

  /// returns a vector of pointers to the sources contained in this group as ModifiableSource interfaces
  virtual const std::vector<std::shared_ptr<ModifiableSource>> getModifiableSources();

  /// Gets a reference to a property
  template<class T, typename... Ts>
  T& getProperty(Ts&&... params) const {
    return dynamic_cast<T&>(getPropertyImpl(PropertyId {typeid(T), std::forward<Ts>(params)...} ));
  }

  /// Sets a property, overwriting it if it exists
  template<class T, typename... Ts>
  void setProperty(std::unique_ptr<T> property, Ts&&... params) {
    setPropertyImpl(std::move(property), PropertyId(typeid(T), std::forward<Ts>(params)...) );
  }

protected:
  // implementation actual implementation used by the getProperty() and setProperty() methods
  virtual Property& getPropertyImpl(const PropertyId property_id) const override;
  virtual void setPropertyImpl(std::unique_ptr<Property> property, PropertyId property_id) override;

private:
  EntangledSourceGroup(std::shared_ptr<TaskRegistry> task_registry);
  void setSources(std::list<std::shared_ptr<Source>> sources);

  std::vector<std::shared_ptr<EntangledSource>> m_sources;
  std::shared_ptr<TaskRegistry> m_task_registry;

  friend class Source;

}; /* End of EntangledSourceGroup class */

} /* namespace SEFramework */


#endif

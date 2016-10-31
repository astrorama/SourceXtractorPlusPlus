/* 
 * @file SourceGroupInterface.h
 * @author nikoapos
 */

#ifndef _SEFRAMEWORK_SOURCEGROUPINTERFACE_H
#define _SEFRAMEWORK_SOURCEGROUPINTERFACE_H

#include "SEFramework/Source/SourceInterface.h"

namespace SExtractor {

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
  
  template <typename T>
  class GroupIterator;
  using iterator = GroupIterator<SourceInterface>;
  using const_iterator = GroupIterator<const SourceInterface>;
  
  virtual iterator begin() = 0;
  virtual iterator end() = 0;
  virtual const_iterator cbegin() = 0;
  virtual const_iterator cend() = 0;
  virtual const_iterator begin() const = 0;
  virtual const_iterator end() const = 0;
  
  virtual void addSource(std::shared_ptr<SourceInterface> source) = 0;
  virtual iterator removeSource(iterator pos) = 0;
  virtual void merge(const SourceGroupInterface& other) = 0;
  
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
  // implementations must implemented the protected methods with the PropertyId
  // in their signature.
  using SourceInterface::getProperty;
  using SourceInterface::setProperty;
  using SourceInterface::setIndexedProperty;
  
protected:
  
  class IteratorImpl {
  public:
    virtual ~IteratorImpl() = default;
    virtual SourceInterface& dereference() const = 0;
    virtual void increment() = 0;
    virtual void decrement() = 0;
    virtual bool equal(const IteratorImpl& other) const = 0;
    virtual std::shared_ptr<IteratorImpl> clone() const = 0;
  };
  
}; // end of SourceGroupInterface class 


template <typename T>
class SourceGroupInterface::GroupIterator : public std::iterator<std::forward_iterator_tag, T> {
public:
  GroupIterator(std::unique_ptr<IteratorImpl> it) : m_it(std::move(it)) { }

  GroupIterator(const GroupIterator& other) : m_it(other.m_it->clone()) {}
  GroupIterator& operator=(const GroupIterator& other) { m_it = other.m_it->clone(); return *this; }

  T& operator*() const { return m_it->dereference(); }
  T* operator->() const { return &(m_it->dereference()); }
  GroupIterator& operator++() { m_it->increment(); return *this; }
  GroupIterator& operator--() { m_it->decrement(); return *this; }
  bool operator==(const GroupIterator& other) const { return m_it->equal(*other.m_it); }
  bool operator!=(const GroupIterator& other) const { return !m_it->equal(*other.m_it); }
  IteratorImpl& getImpl() { return *m_it; }
private:
  std::shared_ptr<IteratorImpl> m_it;
}; // end of SourceGroupInterface::GroupIterator

} /* namespace SExtractor */

#endif /* _SEFRAMEWORK_SOURCEGROUPINTERFACE_H */


/* 
 * @file SimpleSourceGroup.h
 * @author nikoapos
 */

#ifndef _SEFRAMEWORK_SIMPLESOURCEGROUP_H
#define _SEFRAMEWORK_SIMPLESOURCEGROUP_H

#include <set>

#include "SEFramework/Source/SourceGroupInterface.h"
#include "SEFramework/Property/PropertyHolder.h"

namespace SExtractor {

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
  
  void merge(const SourceGroupInterface& other) override;
  
  using SourceInterface::getProperty;
  using SourceInterface::setProperty;

protected:
  
  const Property& getProperty(const PropertyId& property_id) const override;

  void setProperty(std::unique_ptr<Property> property, const PropertyId& property_id) override;
  
private:
  
  class iter;
  std::set<std::shared_ptr<SourceInterface>> m_sources;
  PropertyHolder m_property_holder;
  
}; /* End of SimpleSourceGroup class */


class SimpleSourceGroup::iter : public SourceGroupInterface::IteratorImpl {
  
public:
  iter(std::set<std::shared_ptr<SourceInterface>>::iterator wrapped_it)
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

private:
  
  std::set<std::shared_ptr<SourceInterface>>::iterator m_wrapped_it;
  
  friend SimpleSourceGroup::iterator SimpleSourceGroup::removeSource(SimpleSourceGroup::iterator);
  
};

} /* namespace SExtractor */

#endif /* _SEFRAMEWORK_SIMPLESOURCEGROUP_H */


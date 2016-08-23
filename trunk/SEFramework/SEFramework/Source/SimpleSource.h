/*
 * SimpleSource.h
 *
 *  Created on: Aug 9, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_SOURCE_SIMPLESOURCE_H_
#define _SEFRAMEWORK_SOURCE_SIMPLESOURCE_H_

#include "SEFramework/Source/SourceInterface.h"
#include "SEFramework/Property/PropertyHolder.h"

namespace SExtractor {

/**
 * @class SimpleSource
 * @brief A trivial implementation of SourceInterface meant to be used in unit tests
 *
 * @details Unlike SourceWithOnDemandproperties, SimpleSource doesn't do any computation of properties but is
 *          instead just a simple container for properties.
 *
 */

class SimpleSource : public SourceInterface {

public:

  /// Destructor
  virtual ~SimpleSource() = default;

  // removes copy/move constructors and assignment operators
  SimpleSource(const SimpleSource&) = delete;
  SimpleSource& operator=(const SimpleSource&) = delete;
  SimpleSource(SimpleSource&&) = delete;
  SimpleSource& operator=(SimpleSource&&) = delete;

  /// Constructor
  SimpleSource() {}

  // Note : Because the get/setProperty() methods of the SourceInterface are
  // templated, the overrides of the non-templated versions will hide them. For
  // this reason it is necessary to re-introduce the templated methods, which is
  // done by the using statements below.
  using SourceInterface::getProperty;
  using SourceInterface::setProperty;

protected:

  // Implementation of SourceInterface
  virtual const Property& getProperty(const PropertyId& property_id) const override {
    return m_property_holder.getProperty(property_id);
  }

  virtual void setProperty(std::unique_ptr<Property> property, const PropertyId& property_id) override {
    m_property_holder.setProperty(std::move(property), property_id);
  }

private:
  PropertyHolder m_property_holder;
};

}

#endif /* _SEFRAMEWORK_SOURCE_SIMPLESOURCE_H_ */

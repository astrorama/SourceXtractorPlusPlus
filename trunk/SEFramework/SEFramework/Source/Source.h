/**
 * @file SEFramework/Source/Source.h
 * @date 05/04/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_SOURCE_SOURCE_H
#define _SEFRAMEWORK_SOURCE_SOURCE_H

#include <vector>

#include "SEFramework/Property/PropertyHolder.h"
#include "SEFramework/Source/SourceInterface.h"

#include "SEFramework/History/History.h"

namespace SExtractor {

class TaskProvider;

/**
 * @class Source
 * @brief Source containing pixel detection information.
 *
 * @details Provides getProperty() and setProperty() implementations
 *
 */

class Source : public SourceInterface, public History {

public:

  /**
   * @brief Destructor
   */
  virtual ~Source() = default;

  // removes copy/move constructors and assignment operators

  Source(const Source&) = delete;
  Source& operator=(const Source&) = delete;
  Source(Source&&) = delete;
  Source& operator=(Source&&) = delete;

  /// Constructor
  Source(std::shared_ptr<const TaskProvider> task_provider);

  // Note : Because the get/setProperty() methods of the SourceInterface are
  // templated, the overrides of the non-templated versions will hide them. For
  // this reason it is necessary to re-introduce the templated methods, which is
  // done by the using statements below.
  using SourceInterface::getProperty;
  using SourceInterface::setProperty;
  
protected:
  
  // Implementation of SourceInterface
  virtual const Property& getProperty(const PropertyId& property_id) const override;
  virtual void setProperty(std::unique_ptr<Property> property, const PropertyId& property_id) override;

private:
  std::shared_ptr<const TaskProvider> m_task_provider;
  PropertyHolder m_property_holder;
}; /* End of Source class */

} /* namespace SExtractor */

#endif

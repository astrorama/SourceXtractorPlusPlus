/**
 * @file SEFramework/Source/SourceWithOnDemandProperties.h
 * @date 05/04/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_SOURCE_SOURCE_H
#define _SEFRAMEWORK_SOURCE_SOURCE_H

#include <vector>

#include "SEFramework/Property/PropertyHolder.h"
#include "SEFramework/Source/SourceInterface.h"

namespace SExtractor {

class TaskProvider;

/**
 * @class SourceWithOnDemandProperties
 *
 * @details Provides getProperty() and setProperty() implementations which use
 * a TaskProvider to compute the non already available properties on demand.
 *
 */

class SourceWithOnDemandProperties : public SourceInterface {

public:

  /**
   * @brief Destructor
   */
  virtual ~SourceWithOnDemandProperties() = default;

  // removes copy/move constructors and assignment operators

  SourceWithOnDemandProperties(const SourceWithOnDemandProperties&) = delete;
  SourceWithOnDemandProperties& operator=(const SourceWithOnDemandProperties&) = delete;
  SourceWithOnDemandProperties(SourceWithOnDemandProperties&&) = delete;
  SourceWithOnDemandProperties& operator=(SourceWithOnDemandProperties&&) = delete;

  /// Constructor
  SourceWithOnDemandProperties(std::shared_ptr<const TaskProvider> task_provider);

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

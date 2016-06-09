/**
 * @file SEFramework/Source/Source.h
 * @date 05/04/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_SOURCE_SOURCE_H
#define _SEFRAMEWORK_SOURCE_SOURCE_H

#include "SEUtils/PixelCoordinate.h"
#include "SEFramework/Property/ObjectWithProperties.h"

#include "ModifiableSource.h"
#include "EntangledSourceGroup.h"


namespace SEFramework {

class TaskRegistry;

/**
 * @class Source
 * @brief Source containing pixel detection information.
 *
 * @details Provides getPropertyImpl() and setPropertyImpl implementations
 *
 */

class Source : public ModifiableSource, protected ObjectWithProperties {

public:

  /**
   * @brief Destructor
   */
  virtual ~Source() = default;

  /// Constructor
  Source(std::vector<SEUtils::PixelCoordinate> pixels, std::shared_ptr<const TaskRegistry> task_registry);

  const std::vector<SEUtils::PixelCoordinate>& getPixels() const {
    return m_pixels;
  }

protected:
  // Implementation of ModifiableSource
  virtual Property& getPropertyImpl(const PropertyId property_id) const override;
  virtual void setPropertyImpl(std::unique_ptr<Property> property, PropertyId property_id) override;

private:
  std::shared_ptr<const TaskRegistry> m_task_registry;
  std::vector<SEUtils::PixelCoordinate> m_pixels;

  friend class EntangledSourceGroup::EntangledSource;
}; /* End of Source class */

} /* namespace SEFramework */

#endif

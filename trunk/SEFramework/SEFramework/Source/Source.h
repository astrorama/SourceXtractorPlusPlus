/**
 * @file SEFramework/Source/Source.h
 * @date 05/04/16
 * @author mschefer
 */

#ifndef _SEFRAMEWORK_SOURCE_SOURCE_H
#define _SEFRAMEWORK_SOURCE_SOURCE_H

#include <vector>

#include "SEUtils/PixelCoordinate.h"
#include "SEFramework/Property/PropertyHolder.h"
#include "SEFramework/Source/SourceInterface.h"

#include "SEFramework/History/History.h"

namespace SExtractor {

class TaskRegistry;

/**
 * @class Source
 * @brief Source containing pixel detection information.
 *
 * @details Provides getProperty() and setProperty implementations
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
  Source(std::vector<PixelCoordinate> pixels, std::shared_ptr<const TaskRegistry> task_registry);

  /// Gets a vector of PixelCoordinates for all the pixels in this source
  const std::vector<PixelCoordinate>& getPixels() const {
    return m_pixels;
  }

  // Implementation of SourceInterface
  using SourceInterface::getProperty;
  virtual const Property& getProperty(const PropertyId& property_id) const override;

  using SourceInterface::setProperty;
  virtual void setProperty(std::unique_ptr<Property> property, const PropertyId& property_id) override;

private:
  PropertyHolder m_property_holder;
  std::vector<PixelCoordinate> m_pixels;

  std::shared_ptr<const TaskRegistry> m_task_registry;
}; /* End of Source class */

} /* namespace SExtractor */

#endif

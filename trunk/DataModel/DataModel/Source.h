/**
 * @file DataModel/Source.h
 * @date 05/04/16
 * @author mschefer
 */

#ifndef _DATAMODEL_SOURCE_H
#define _DATAMODEL_SOURCE_H

#include "ModifiableSource.h"
#include "ObjectWithProperties.h"

#include "EntangledSourceGroup.h"

namespace DataModel {

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
  Source(std::shared_ptr<TaskRegistry> task_registry);

protected:
  // Implementation of ModifiableSource
  virtual Property& getPropertyImpl(const PropertyId property_id) const override;
  virtual void setPropertyImpl(std::unique_ptr<Property> property, PropertyId property_id) override;

private:
  std::shared_ptr<TaskRegistry> m_task_registry;

  friend class EntangledSourceGroup::EntangledSource;
}; /* End of Source class */

} /* namespace DataModel */


#endif

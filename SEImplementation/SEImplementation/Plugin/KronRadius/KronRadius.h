/*
 * AutoPhotometry.h
 *
 *  Created on: Sept. 12, 2018
 *      Author: mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_KRONRADIUS_KRONRADIUS_H_
#define _SEIMPLEMENTATION_PLUGIN_KRONRADIUS_KRONRADIUS_H_

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"

namespace SExtractor {

/**
 * @class KronRadius
 * @brief Kron radius
 */
class KronRadius : public Property {
public:

  /**
   * @brief Destructor
   */
  virtual ~KronRadius() = default;

  KronRadius(SeFloat kron_radius)
    : m_kron_radius(kron_radius) {}

  SeFloat getKronRadius() const {
    return m_kron_radius;
  }

private:
  SeFloat m_kron_radius;
};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_PLUGIN_KRONRADIUS_KRONRADIUS_H_ */

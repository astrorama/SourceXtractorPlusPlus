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

  KronRadius(SeFloat kron_radius, long int flag)
    : m_kron_radius(kron_radius), m_flag(flag) {}

  SeFloat getKronRadius() const {
    return m_kron_radius;
  }

  SeFloat getFlag() const {
    return m_flag;
  }

private:
  SeFloat  m_kron_radius;
  long int m_flag;
};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_PLUGIN_KRONRADIUS_KRONRADIUS_H_ */

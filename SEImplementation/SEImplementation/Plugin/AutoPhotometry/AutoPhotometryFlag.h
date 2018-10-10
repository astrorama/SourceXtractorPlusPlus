/*
 * AutoPhotometryFlag.h
 *
 *  Created on: Oct 10, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYFLAG_H_
#define _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYFLAG_H_

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"

namespace SExtractor {

class AutoPhotometryFlag: public Property {
public:
  virtual ~AutoPhotometryFlag() = default;

  AutoPhotometryFlag(long int flag): m_flag{flag} {}

  long int getFlag() const {
    return m_flag;
  }

private:
  long int m_flag;
};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYFLAG_H_ */

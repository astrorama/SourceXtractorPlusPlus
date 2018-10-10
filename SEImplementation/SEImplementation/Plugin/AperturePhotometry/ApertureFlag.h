/*
 * ApertureFlag.h
 *
 *  Created on: Oct 09, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREFLAG_H_
#define _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREFLAG_H_

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"

namespace SExtractor {

/**
 * @class ApertureFlag
 * @brief Aperture photometry flag
 */
class ApertureFlag : public Property {
public:

  virtual ~ApertureFlag() = default;

  ApertureFlag(int flag): m_flag{flag} {}

  int getFlag() const {
    return m_flag;
  }

private:
  int m_flag;
};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREFLAG_H_ */

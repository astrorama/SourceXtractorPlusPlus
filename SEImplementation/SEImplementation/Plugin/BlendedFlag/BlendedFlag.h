/*
 * BlendedFlag.h
 *
 *  Created on: Oct 29, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_BLENDEDFLAG_H_
#define _SEIMPLEMENTATION_PLUGIN_BLENDEDFLAG_H_

#include "SEFramework/Property/Property.h"

namespace SExtractor {

class BlendedFlag : public Property {
public:
  virtual ~BlendedFlag() = default;

  BlendedFlag(bool blended_flag) : m_blended_flag{blended_flag} {}

  bool getBlendedFlag() const {
    return m_blended_flag;
  }

private:
  bool m_blended_flag;
};

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_BLENDEDFLAG_H_

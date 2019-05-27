/*  
 * Copyright (C) 2012-2020 Euclid Science Ground Segment    
 *  
 * This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General  
 * Public License as published by the Free Software Foundation; either version 3.0 of the License, or (at your option)  
 * any later version.  
 *  
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied  
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more  
 * details.  
 *  
 * You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to  
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA  
 */    

/**
 * @file SnrLevel.h
 *
 * @date May 27, 2019
 * @author mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_SNRLEVEL_H_
#define _SEIMPLEMENTATION_PLUGIN_SNRLEVEL_H_

#include "SEFramework/Property/Property.h"

namespace SExtractor {
class SnrLevel : public Property {
public:
  virtual ~SnrLevel() = default;
  SnrLevel(unsigned long n_snr_level) : m_n_snr_level(n_snr_level) {}
  long int getSnrLevel () const {
    return m_n_snr_level;
  }
private:
  long int m_n_snr_level;
}; // end of SnrLevel class
} // namespace SExtractor

#endif /* _SEIMPLEMENTATION_PLUGIN_SNRLEVEL_H_*/

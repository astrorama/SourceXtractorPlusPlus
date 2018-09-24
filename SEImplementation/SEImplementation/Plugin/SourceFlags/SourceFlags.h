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
 * @file SourceFlags.h
 *
 * @date Jul 12, 2018
 * @author mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_SOURCEFLAGS_H_
#define _SEIMPLEMENTATION_PLUGIN_SOURCEFLAGS_H_

#include "SEFramework/Property/Property.h"

namespace SExtractor {

class SourceFlags : public Property {
public:
  // Backwards compatible with sextractor flags!
  static const int HAS_NEIGHBORS = 1 << 0;
  static const int BLENDED = 1 << 1;
  static const int SATURATED = 1 << 2;
  static const int BOUNDARY = 1 << 3;

  virtual ~SourceFlags() = default;
  SourceFlags(unsigned long source_flags) : m_source_flags(source_flags) {}
  long int getSourceFlags () const {
    return m_source_flags;
  }

private:
  long int m_source_flags;
}; // end of SourceFlags class

} // namespace SExtractor

#endif /* _SEIMPLEMENTATION_PLUGIN_SOURCEFLAGS_H_*/

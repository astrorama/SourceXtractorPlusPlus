/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/* 
 * @file ObjectInfo.h
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#ifndef _SEIMPLEMENTATION_OBJECTINFO_H
#define _SEIMPLEMENTATION_OBJECTINFO_H

#include <functional>
#include <SEUtils/Types.h>
#include <SEFramework/Source/SourceInterface.h>

namespace SExtractor {

class ObjectInfo {
  
public:
  
  ObjectInfo(const SourceInterface& source);
  
  virtual ~ObjectInfo() = default;
  
  SeFloat getCentroidX() const;
  
  SeFloat getCentroidY() const;
  
  SeFloat getIsoFlux() const;
  
  SeFloat getRadius() const;
  
  SeFloat getAngle() const;

  SeFloat getAspectRatio() const;

private:
  
  std::reference_wrapper<const SourceInterface> m_source;
  
};

} // end of namespace SExtractor

#endif // _SEIMPLEMENTATION_OBJECTINFO_H


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

/**
 * @file Vignet.h
 *
 * @date Jan 17, 2020
 * @author mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_VIGNET_H_
#define _SEIMPLEMENTATION_PLUGIN_VIGNET_H_

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"
#include "SEFramework/Image/VectorImage.h"

namespace SourceXtractor {
class Vignet : public Property {
public:
  virtual ~Vignet() = default;

  Vignet(const std::shared_ptr<VectorImage<DetectionImage::PixelType>> vignet) : m_vignet(vignet) {}

  const VectorImage<DetectionImage::PixelType>& getVignet() const {
    return *m_vignet;
  }

private:
  std::shared_ptr<VectorImage<DetectionImage::PixelType>> m_vignet;
}; // end of Vignet class

} // namespace SourceXtractor

#endif /* _SEIMPLEMENTATION_PLUGIN_VIGNET_H_*/

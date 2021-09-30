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

#ifndef _SEIMPLEMENTATION_PLUGIN_SOURCEPSF_SOURCEPSF_H_
#define _SEIMPLEMENTATION_PLUGIN_SOURCEPSF_SOURCEPSF_H_

#include <SEFramework/Property/Property.h>
#include <SEFramework/Image/VectorImage.h>

namespace SourceXtractor {

class SourcePsfProperty : public Property {
public:
  virtual ~SourcePsfProperty() = default;

  SourcePsfProperty(double pixel_sampling, std::shared_ptr<VectorImage <SeFloat>> psf) :
    m_pixel_sampling(pixel_sampling), m_psf(psf) {};

  SourcePsfProperty();

  double getPixelSampling() const {
    return m_pixel_sampling;
  }

  std::shared_ptr<VectorImage<SeFloat>> getPsf() const {
    return m_psf;
  }

private:
  double m_pixel_sampling;
  std::shared_ptr<VectorImage<SeFloat>> m_psf;
};

} // end SourceXtractor

#endif //_SEIMPLEMENTATION_PLUGIN_SOURCEPSF_SOURCEPSF_H_

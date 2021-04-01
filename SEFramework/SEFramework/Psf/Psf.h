/** Copyright © 2019-2012 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#ifndef _SEIMPLEMENTATION_PSF_PSF_H_
#define _SEIMPLEMENTATION_PSF_PSF_H_

#include "SEFramework/Image/VectorImage.h"
#include <string>
#include <vector>

namespace SourceXtractor {

class Psf {
public:
  /**
   * Destructor
   */
  virtual ~Psf() = default;

  /**
   * @return The width of the PSF
   */
  virtual int getWidth() const = 0;

  /**
   * @return The height of the PSF
   */
  virtual int getHeight() const = 0;

  /**
   * @return The pixel sampling
   */
  virtual double getPixelSampling() const = 0;

  /**
   * @return A reference to the list of component names
   */
  virtual const std::vector<std::string>& getComponents() const = 0;

  /**
   * Reconstructs a PSF based on the given values for each of the components.
   * @param values
   *    Component values. Note that they have to be in the same order (and as many)
   *    as components were passed to the constructor (none for constant PSF).
   * @return
   *    The reconstructed PSF
   * @throws
   *    If the number of values does not match the number of components
   */
  virtual std::shared_ptr<VectorImage<SeFloat>> getPsf(const std::vector<double>& values) const = 0;
};

}  // namespace SourceXtractor

#endif  // _SEIMPLEMENTATION_PSF_PSF_H_

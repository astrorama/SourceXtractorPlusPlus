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
 * VariablePsf.h
 *
 *  Created on: Jun 25, 2018
 *      Author:
 */

#ifndef _SEIMPLEMENTATION_PSF_VARIABLEPSFSTACK_H_
#define _SEIMPLEMENTATION_PSF_VARIABLEPSFSTACK_H_

#include <CCfits/CCfits>
#include <SEFramework/Image/VectorImage.h>
#include <SEFramework/Psf/Psf.h>

namespace SourceXtractor {

/**
 * @class VariablePsfStack
 *
 * @brief
 *
 * @details
 *
 * @todo
 *
 */
class VariablePsfStack final : public Psf {
public:
  /**
   * Constructor
   */
  explicit VariablePsfStack(std::shared_ptr<CCfits::FITS> pFits) : m_pFits(pFits), m_psf_size(0), mm_pixel_sampling(0.0) {
    setup(pFits);
    selfTest();
  };

  /**
   * Destructor
   */
  virtual ~VariablePsfStack() = default;

  /**
   * @return The width of the PSF
   */
  int getWidth() const {
    return m_psf_size;
  };

  /**
   * @return The height of the PSF
   */
  int getHeight() const {
    return m_psf_size;
  };

  /**
   * @return The pixel scale, as passed to the constructor
   */
  double getPixelSampling() const {
    return mm_pixel_sampling;
  };

  /**
   * @return A reference to the list of components
   */
  const std::vector<std::string>& getComponents() const {
    return m_components;
  };

  /**
   *
   */
  virtual std::shared_ptr<VectorImage<SeFloat>> getPsf(const std::vector<double>& values) const;

private:
  mutable std::mutex m_mutex;
  std::shared_ptr<CCfits::FITS> m_pFits;

  int m_psf_size;
  int m_grid_offset;

  double mm_pixel_sampling;

  long m_nrows;

  std::vector<SeFloat> m_ra_values;
  std::vector<SeFloat> m_dec_values;
  std::vector<SeFloat> m_x_values;
  std::vector<SeFloat> m_y_values;
  std::vector<int>     m_gridx_values;
  std::vector<int>     m_gridy_values;

  std::vector<std::string> m_components = {"X_IMAGE", "Y_IMAGE"};

  /*
   * Check the file, load the positions and so on
   */
  void setup(std::shared_ptr<CCfits::FITS> pFits);

  /*
   * consistency of the stackedPSF
   */
  void selfTest();
};

}  // namespace SourceXtractor

#endif  //_SEIMPLEMENTATION_PSF_VARIABLEPSFSTACK_H_

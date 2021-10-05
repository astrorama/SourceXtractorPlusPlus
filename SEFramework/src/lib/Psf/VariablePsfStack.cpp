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
 * VariablePsfStack.cpp
 *
 *  Created on: July 10, 2019
 *      Author: Martin Kuemmel
 */
#include <algorithm>
#include <ElementsKernel/Logging.h>
#include <ElementsKernel/Exception.h>
#include "SEFramework/Psf/VariablePsfStack.h"

static auto stack_logger = Elements::Logging::getLogger("VarStackPsf");

namespace SourceXtractor {

void VariablePsfStack::setup(std::shared_ptr<CCfits::FITS> pFits) {
  try {
    // basic check: load the primary HDU
    CCfits::PHDU& phdu = pFits->pHDU();
    if (phdu.axes() != 0) {
      throw Elements::Exception() << "The primary HDU is not empty! File: " << pFits->name();
    }

    // basic check: load the first extension
    CCfits::ExtHDU& psf_data = pFits->extension(1);
    if (psf_data.axes() != 2) {
      throw Elements::Exception() << "The first HDU is not an image! File: " << pFits->name();
    }

    // basic check: load the second extension
    CCfits::ExtHDU& position_data = pFits->extension(2);
    if (position_data.axes() != 2) {
      throw Elements::Exception() << "The second HDU has not the right dimension! File: " << pFits->name();
    }

    // give some feedback
    stack_logger.debug() << "Checked the file: " << pFits->name();

    // get and store the stamp size;
    // define the offset from GRIDX and GRIDY
    psf_data.readKey("STMPSIZE", m_psf_size);
    m_grid_offset = m_psf_size / 2;  // this is for CCfits where indices start with 1!

    // make sure the PSF size is odd
    if (m_psf_size % 2 == 0) {
      throw Elements::Exception() << "PSF kernel must have odd size, but has: " << m_psf_size;
    }

    try {
      // try to get the sampling
      psf_data.readKey("SAMPLING", mm_pixel_sampling);
    } catch (CCfits::HDU::NoSuchKeyword&) {
      // use a default value
      mm_pixel_sampling = 1.;
    }

    // read the nrows value
    m_nrows = position_data.rows();

    try {
      // read in all the EXT specific columns
      position_data.column("GRIDX", false).read(m_gridx_values, 0, m_nrows);
      position_data.column("GRIDY", false).read(m_gridy_values, 0, m_nrows);
    } catch (CCfits::Table::NoSuchColumn) {
      position_data.column("X_CENTER", false).read(m_gridx_values, 0, m_nrows);
      position_data.column("Y_CENTER", false).read(m_gridy_values, 0, m_nrows);
    }
    position_data.column("RA", false).read(m_ra_values, 0, m_nrows);
    position_data.column("DEC", false).read(m_dec_values, 0, m_nrows);
    position_data.column("X", false).read(m_x_values, 0, m_nrows);
    position_data.column("Y", false).read(m_y_values, 0, m_nrows);

  } catch (CCfits::FitsException& e) {
    throw Elements::Exception() << "Error loading stacked PSF file: " << e.message();
  }
}

void VariablePsfStack::selfTest() {
  int naxis1, naxis2;

  // read in the min/max grid values in x/y
  const auto x_grid_minmax = std::minmax_element(begin(m_gridx_values), end(m_gridx_values));
  const auto y_grid_minmax = std::minmax_element(begin(m_gridy_values), end(m_gridy_values));

  // read the image size
  m_pFits->extension(1).readKey("NAXIS1", naxis1);
  m_pFits->extension(1).readKey("NAXIS2", naxis2);

  // make sure all PSF in the grid are there
  if (*x_grid_minmax.first - m_grid_offset < 1)
    throw Elements::Exception() << "The PSF at the smallest x-grid starts at: " << *x_grid_minmax.first - m_grid_offset;
  if (*y_grid_minmax.first - m_grid_offset < 1)
    throw Elements::Exception() << "The PSF at the smallest y-grid starts at: " << *y_grid_minmax.first - m_grid_offset;
  if (*x_grid_minmax.second + m_grid_offset > naxis1)
    throw Elements::Exception() << "The PSF at the largest x-grid is too large: " << *x_grid_minmax.second + m_grid_offset
                                << " NAXIS1: " << naxis1;
  if (*y_grid_minmax.second + m_grid_offset > naxis2)
    throw Elements::Exception() << "The PSF at the largest y-grid is too large: " << *y_grid_minmax.second + m_grid_offset
                                << " NAXIS2: " << naxis1;
}

std::shared_ptr<VectorImage<SeFloat>> VariablePsfStack::getPsf(const std::vector<double>& values) const {
  long   index_min_distance = 0;
  double min_distance       = 1.0e+32;

  // make sure there are only two positions
  if (values.size() != 2)
    throw Elements::Exception() << "There can be only two positional value for the stacked PSF!";

  // find the position of minimal distance
  for (int act_index = 0; act_index < m_nrows; act_index++) {
    double act_distance = (values[0] - m_x_values[act_index]) * (values[0] - m_x_values[act_index]) +
                          (values[1] - m_y_values[act_index]) * (values[1] - m_y_values[act_index]);
    if (act_distance < min_distance) {
      index_min_distance = act_index;
      min_distance       = act_distance;
    }
  }
  // give some feedback
  stack_logger.debug() << "Distance: " << sqrt(min_distance) << " (" << values[0] << "," << values[1] << ")<-->("
                       << m_x_values[index_min_distance] << "," << m_y_values[index_min_distance]
                       << ") index: " << index_min_distance;

  // get the first and last pixels for the PSF to be extracted
  // NOTE: CCfits has 1-based indices, also the last index is *included* in the reading
  std::vector<long> first_vertex = {m_gridx_values[index_min_distance] - m_grid_offset,
                                    m_gridy_values[index_min_distance] - m_grid_offset};
  std::vector<long> last_vertex  = {first_vertex[0] + m_psf_size - 1, first_vertex[1] + m_psf_size - 1};
  std::vector<long> stride       = {1, 1};

  // read out the image
  std::valarray<SeFloat> stamp_data;
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pFits->extension(1).read(stamp_data, first_vertex, last_vertex, stride);
  }

  // create and return the psf image
  return VectorImage<SeFloat>::create(m_psf_size, m_psf_size, std::begin(stamp_data), std::end(stamp_data));
}

} // end SExtractor

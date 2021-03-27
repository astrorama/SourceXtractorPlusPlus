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

#include <ElementsKernel/Logging.h>
#include <ElementsKernel/Exception.h>
#include "SEFramework/Psf/VariablePsfStack.h"

static auto stack_logger = Elements::Logging::getLogger("VarStackPsf");

namespace SourceXtractor {

void VariablePsfStack::setup(std::shared_ptr<CCfits::FITS> pFits){

  try {
    // basic check: load the primary HDU
    CCfits::PHDU &phdu = pFits->pHDU();
    if (phdu.axes() != 0){
      throw Elements::Exception() << "The primary HDU is not empty! File: " << pFits->name();
    }

    // basic check: load the first extension
    CCfits::ExtHDU &psf_data = pFits->extension(1);
    if (psf_data.axes() != 2){
      throw Elements::Exception() << "The first HDU is not an image! File: " << pFits->name();
    }

    // basic check: load the second extension
    CCfits::ExtHDU &position_data = pFits->extension(2);
    if (position_data.axes() != 2){
      throw Elements::Exception() << "The second HDU has not the right dimension! File: " << pFits->name();
    }

    // give some feedback
    stack_logger.info() << "Checked the file: " << pFits->name();

    // get and store the stamp size;
    // define the offset from GRIDX and GRIDY
    psf_data.readKey("STMPSIZE", m_psf_size);
    m_grid_offset = m_psf_size / 2; // this is for CCfits where indices start with 1!

    // make sure the PSF size is odd
    if (m_psf_size % 2 == 0) {
      throw Elements::Exception() << "PSF kernel must have odd size, but has: " << m_psf_size;
    }

    try {
      // try to get the sampling
      psf_data.readKey("SAMPLING", m_pixel_sampling);
    }
    catch (CCfits::HDU::NoSuchKeyword&) {
      // use a default value
      m_pixel_sampling = 1.;
    }

    // read the nrows value
    m_nrows = position_data.rows();

    // read in all the positions
    position_data.column("RA").read(m_ra_values, 0, m_nrows);
    position_data.column("DEC").read(m_dec_values, 0, m_nrows);
    position_data.column("X").read(m_x_values, 0, m_nrows);
    position_data.column("Y").read(m_y_values, 0, m_nrows);
    position_data.column("GRIDX").read(m_gridx_values, 0, m_nrows);
    position_data.column("GRIDY").read(m_gridy_values, 0, m_nrows);

    // give some feedback
    stack_logger.info() << "Read in "<< m_nrows << " psf positions!";

  } catch (CCfits::FitsException &e) {
    throw Elements::Exception() << "Error loading stacked PSF file: " << e.message();
  }
}

std::shared_ptr<VectorImage<SeFloat>> VariablePsfStack::getPsf(const std::vector<double> &values) const {
  long index_min_distance=0;
  double min_distance=1.0e+32;
  std::vector<double> m_values = {100.,100.};
  stack_logger.info() << "In VariablePsfStack::stack!";
  stack_logger.info() << "size: " << values.size();
  for (int index=0; index<values.size(); index++)
    stack_logger.info() << "value: " << values[index];

  // make sure there are only two positions
  //if (values.size()!=2)
  //  throw Elements::Exception() << "There can be only two positional value for the stacked PSF!";
  if (values.size()==2){
      m_values[0] = values[0];
      m_values[1] = values[1];
  }

  // find the position of minimal distance
  for (int act_index=0; act_index < m_nrows; act_index++){
      //double act_distance = (values[0]-m_x_values[act_index])*(values[0]-m_x_values[act_index]) + (values[1]-m_y_values[act_index])*(values[1]-m_y_values[act_index]);
      double act_distance = (m_values[0]-m_x_values[act_index])*(m_values[0]-m_x_values[act_index]) + (m_values[1]-m_y_values[act_index])*(m_values[1]-m_y_values[act_index]);
    if (act_distance < min_distance){
      index_min_distance = act_index;
      min_distance = act_distance;
    }
  }
  // give some feedback
  stack_logger.info() << "The minimum distance is: "<< sqrt(min_distance) << " at position index: " << index_min_distance;

  // get the first and last pixels for the PSF to be extracted
  // NOTE: CCfits has 1-based indices, also the last index is *included* in the reading
  std::vector<long> first_vertex = {m_gridx_values[index_min_distance] - m_grid_offset, m_gridy_values[index_min_distance] - m_grid_offset};
  std::vector<long> last_vertex  = {first_vertex[0] + m_psf_size - 1, first_vertex[1] + m_psf_size - 1};
  std::vector<long> stride = {1, 1};

  // read out the image
  std::valarray<SeFloat> stamp_data;
  m_pFits->extension(1).read (stamp_data, first_vertex, last_vertex, stride);
  //stack_logger.info() << "first vertex: "<< first_vertex[0] << " " << first_vertex[1];
  stack_logger.info() << "Out VariablePsfStack::stack!";

  // create and return the psf image
  return VectorImage<SeFloat>::create(m_psf_size, m_psf_size, std::begin(stamp_data), std::end(stamp_data));
}

} // end SExtractor

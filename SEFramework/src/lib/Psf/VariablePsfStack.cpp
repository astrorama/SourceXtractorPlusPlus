/*
 * VariablePsfStack.cpp
 *
 *  Created on: July 10, 2019
 *      Author: Martin Kuemmel
 */

#include <ElementsKernel/Logging.h>
#include <ElementsKernel/Exception.h>
//#include <algorithm>
#include "SEFramework/Psf/VariablePsfStack.h"

static auto stack_logger = Elements::Logging::getLogger("VarStackPsf");

namespace SExtractor {

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

    // get and store the stamp size;
    // define the offset from GRIDX and GRIDY
    psf_data.readKey("STMPSIZE", m_psf_size);
    m_grid_offset = m_psf_size / 2; // this is for CCfits where indices start with 1!

    // TODO: store the pixel scale here!
    //psf_data.readKey("STMPSIZE", m_pixel_scale);

    // read the nrows value
    m_nrows = position_data.rows();

    // read in all the positions
    position_data.column("RA").read(m_ra_values, 0, m_nrows);
    position_data.column("DEC").read(m_dec_values, 0, m_nrows);
    position_data.column("X").read(m_x_values, 0, m_nrows);
    position_data.column("Y").read(m_y_values, 0, m_nrows);
    position_data.column("GRIDX").read(m_gridx_values, 0, m_nrows);
    position_data.column("GRIDY").read(m_gridy_values, 0, m_nrows);

  } catch (CCfits::FitsException &e) {
    throw Elements::Exception() << "Error loading stacked PSF file: " << e.message();
  }
}

std::shared_ptr<VectorImage<SeFloat>> VariablePsfStack::getPsf(const std::vector<double> &values) const {
  long index_min_distance=0;
  double min_distance=1.0e+32;

  // find the position of minimal distance
  for (int act_index=0; act_index < m_nrows; act_index++){
    double act_distance = (values[0]-m_x_values[act_index])*(values[0]-m_x_values[act_index]) + (values[1]-m_y_values[act_index])*(values[1]-m_y_values[act_index]);
    if (act_distance < min_distance){
      index_min_distance = act_index;
      min_distance = act_distance;
    }
  }
  //stack_logger.info() << "index_min_distance: "<< index_min_distance;

  // get the limits for the pixels
  std::vector<long> first_vertex = {m_gridx_values[index_min_distance] - m_grid_offset, m_gridy_values[index_min_distance] - m_grid_offset};
  std::vector<long> last_vertex  = {first_vertex[0] + m_psf_size - 1, first_vertex[1] + m_psf_size - 1};
  std::vector<long> stride = {1, 1};

  // read out the image
  std::valarray<SeFloat> stamp_data;
  m_pFits->extension(1).read (stamp_data, first_vertex, last_vertex, stride);
  //stack_logger.info() << "first vertex: "<< first_vertex[0] << " " << first_vertex[1];

  // create and return the psf image
  return VectorImage<SeFloat>::create(m_psf_size, m_psf_size, std::begin(stamp_data), std::end(stamp_data));
}

} // end SExtractor

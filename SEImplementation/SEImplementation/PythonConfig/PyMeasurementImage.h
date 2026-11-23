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
 * @file MeasurementImage.h
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#ifndef _SEIMPLEMENTATION_PYMEASUREMENTIMAGE_H
#define _SEIMPLEMENTATION_PYMEASUREMENTIMAGE_H

#include <string>
#include "SEFramework/Image/ImageSelector.h"

#include <SEImplementation/PythonConfig/PyId.h>

namespace SourceXtractor {

class PyMeasurementImage : public PyId {

public:
  PyMeasurementImage(std::string file, std::string psf_file, std::string weight_file);

  std::string file;
  double gain;
  double saturation;
  double flux_scale;
  std::string psf_file;
  std::string weight_file;
  std::string weight_type;
  bool weight_absolute;
  double weight_scaling;
  bool has_weight_threshold;
  double weight_threshold;

  bool is_background_constant;
  double constant_background_value;

  /**
   * Which image within the file, and which weight within its own
   *
   * An index or a name, the name taking the same text as a `file[image]`
   * suffix: for ASDF a tree path, for FITS an EXTNAME.
   */
  ImageSelector image;
  ImageSelector weight;

  /**
   * Which HDU of the PSF file to read
   *
   * Still uses HDU index rather than an ImageSelector since a PSF is read by
   * PsfPluginConfig::readPsf through CCfits, never through an ImageFileReader
   * at least for now.  Need to find out what format(s) exist for PSDF in
   * ASDF.
   */
  int psf_hdu;

  /// Which coordinate system to use, for formats that can hold several
  std::string wcs;

  bool is_data_cube;
  int image_layer;
  int weight_layer;

  bool psf_renormalize;
};

}

#endif // _SEIMPLEMENTATION_PYMEASUREMENTIMAGE_H


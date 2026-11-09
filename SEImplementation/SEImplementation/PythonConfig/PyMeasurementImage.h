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

  int image_hdu;
  int psf_hdu;
  int weight_hdu;

  /**
   * Which image within the file, named rather than numbered
   *
   * Takes the same text as a `file[image]` suffix: for ASDF a tree path, for
   * FITS an EXTNAME.  Empty means fall back to the corresponding *_hdu index,
   * which is what a FITS configuration uses.  These mirror the
   * --detection-image-name and --detection-image-wcs-name options.
   */
  std::string image_name;
  std::string psf_name;
  std::string weight_name;

  /// Which coordinate system to use, for formats that can hold several
  std::string wcs_name;

  bool is_data_cube;
  int image_layer;
  int weight_layer;

  bool psf_renormalize;
};

}

#endif // _SEIMPLEMENTATION_PYMEASUREMENTIMAGE_H


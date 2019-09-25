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

namespace SExtractor {

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
};

}

#endif // _SEIMPLEMENTATION_PYMEASUREMENTIMAGE_H


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
 * @file MeasurementImage.cpp
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#include <SEImplementation/PythonConfig/PyMeasurementImage.h>

namespace SourceXtractor {

PyMeasurementImage::PyMeasurementImage(std::string file, std::string psf_file, std::string weight_file)
  : file(file), gain(0.), saturation(0.), flux_scale(1.), psf_file(psf_file), weight_file(weight_file),
    weight_absolute(false), weight_scaling(0.), has_weight_threshold(false), weight_threshold(0.),
    is_background_constant(false), constant_background_value(0.), image_hdu(0), psf_hdu(0), weight_hdu(0) {
}

}

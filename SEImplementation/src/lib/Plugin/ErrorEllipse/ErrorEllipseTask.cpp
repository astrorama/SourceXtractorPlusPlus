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
 * ErrorEllipseTask.cpp
 *
 *  Created on: Feb 11 2022
 *      Author: mkuemmel
 */

#include <cmath>

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
#include "SEImplementation/Plugin/ErrorEllipse/ErrorEllipse.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/PeakValue/PeakValue.h"

#include "SEImplementation/Plugin/ErrorEllipse/ErrorEllipseTask.h"

namespace SourceXtractor {

void ErrorEllipseTask::computeProperties(SourceInterface& source) const {
  const auto& pixel_variances = source.getProperty<DetectionFramePixelValues>().getVariances();
  const auto& centroid_x = source.getProperty<PixelCentroid>().getCentroidX();
  const auto& centroid_y = source.getProperty<PixelCentroid>().getCentroidY();
  const auto& coordinates = source.getProperty<PixelCoordinateList>().getCoordinateList();
  auto total_intensity = source.getProperty<ShapeParameters>().getIntensity();
  auto singu = source.getProperty<ShapeParameters>().getSinguFlag();

  SeFloat x_2_error = 0.0;
  SeFloat y_2_error = 0.0;
  SeFloat x_y_error = 0.0;

  SeFloat total_variance = 0;

  auto j = pixel_variances.begin();
  for (auto pixel_coord : coordinates) {
	SeFloat variance = *j++;
	SeFloat x_pos = pixel_coord.m_x - centroid_x;
	SeFloat y_pos = pixel_coord.m_y - centroid_y;

    x_2_error += x_pos * x_pos * variance;
    y_2_error += y_pos * y_pos * variance;
    x_y_error += x_pos * y_pos * variance;

    total_variance  += variance;
  }

  SeFloat total_intensity_2 = total_intensity * total_intensity;
  x_2_error /= total_intensity_2;
  y_2_error /= total_intensity_2;
  x_y_error /= total_intensity_2;

  //esum *= 0.08333/flux2; // from the original SE2 code
  total_variance *= 0.08333/total_intensity_2;
  // handle fully correlated x/y
  if (singu && (x_2_error*y_2_error-x_y_error*x_y_error)<total_variance*total_variance) {
  	x_2_error += total_variance;
  	y_2_error += total_variance;
  }

  const SeFloat variance_difference = x_2_error - y_2_error;

  // A circular ellipse has no preferred orientation.
  SeFloat theta_error = 0.0;
  if (variance_difference != 0.0 || x_y_error != 0.0) {
    theta_error = 0.5 * std::atan2(2.0 * x_y_error, variance_difference);
  }

  // major/minor axis for the error ellipse
  const SeFloat axis_offset = std::sqrt(0.25 * variance_difference * variance_difference + x_y_error * x_y_error);
  const SeFloat mean_variance = 0.5 * (x_2_error + y_2_error);
  const SeFloat a_error = std::sqrt(static_cast<double>(mean_variance + axis_offset));
  const SeFloat b_error = std::sqrt(static_cast<double>(mean_variance - axis_offset));

  // set the property
  source.setProperty<ErrorEllipse>(a_error, b_error, theta_error, x_2_error, y_2_error, x_y_error);
}


}

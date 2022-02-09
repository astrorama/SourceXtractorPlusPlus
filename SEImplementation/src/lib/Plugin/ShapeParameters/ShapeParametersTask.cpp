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
 * ShapeParametersTask.cpp
 *
 *  Created on: Jan 27, 2017
 *      Author: mschefer
 */

#include <iostream>

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/PeakValue/PeakValue.h"

#include "SEImplementation/Plugin/ShapeParameters/ShapeParametersTask.h"

namespace SourceXtractor {

void ShapeParametersTask::computeProperties(SourceInterface& source) const {
  const auto& pixel_values = source.getProperty<DetectionFramePixelValues>().getFilteredValues();
  const auto& pixel_variances = source.getProperty<DetectionFramePixelValues>().getVariances();
  const auto& centroid_x = source.getProperty<PixelCentroid>().getCentroidX();
  const auto& centroid_y = source.getProperty<PixelCentroid>().getCentroidY();
  auto min_value = source.getProperty<PeakValue>().getMinValue();
  auto peak_value = source.getProperty<PeakValue>().getMaxValue();
  auto& coordinates = source.getProperty<PixelCoordinateList>().getCoordinateList();

  SeFloat x_2 = 0.0;
  SeFloat y_2 = 0.0;
  SeFloat x_y = 0.0;
  SeFloat x_2_error = 0.0;
  SeFloat y_2_error = 0.0;
  SeFloat x_y_error = 0.0;

  SeFloat total_intensity = 0;
  SeFloat total_variance = 0;

  DetectionImage::PixelType half_peak_threshold = (peak_value + min_value) / 2.0;
  int nb_of_pixels_above_half = 0;
  int nb_of_pixels = coordinates.size();

  auto i = pixel_values.begin();
  auto j = pixel_variances.begin();
  for (auto pixel_coord : coordinates) {
	SeFloat value = *i++;
	SeFloat variance = *j++;
	SeFloat x_pos = pixel_coord.m_x - centroid_x;
	SeFloat y_pos = pixel_coord.m_y - centroid_y;

    if (value > half_peak_threshold) {
      nb_of_pixels_above_half++;
    }

    x_2 += x_pos * x_pos * value;
    y_2 += y_pos * y_pos * value;
    x_y += x_pos * y_pos * value;

    //emx2 += err*x*x;
    //emy2 += err*y*y;
    //emxy += err*x*y;
    x_2_error += x_pos * x_pos * variance;
    y_2_error += y_pos * y_pos * variance;
    x_y_error += x_pos * y_pos * variance;

    total_intensity += value;
    total_variance  += variance;
  }

  x_2 /= total_intensity;
  y_2 /= total_intensity;
  x_y /= total_intensity;

  SeFloat total_intensity_2 = total_intensity * total_intensity;
  x_2_error /= total_intensity_2;
  y_2_error /= total_intensity_2;
  x_y_error /= total_intensity_2;
  //esum *= 0.08333/flux2;
  total_variance *= 0.08333/total_intensity_2;

  SeFloat theta;
  if (fabs(x_2 - y_2) > 0.0) {
    theta = atan2(2.0 * x_y, x_2 - y_2) / 2.0;
  } else {
    theta = M_PI/4.0;
  }

  float temp = x_2 - y_2;
  temp = sqrt(0.25 * temp * temp + x_y * x_y);
  SeFloat a = sqrt((x_2 + y_2) / 2 + temp);
  SeFloat b = ((x_2 + y_2) / 2) > temp ? sqrt((x_2 + y_2) / 2 - temp) : 0;

  // From original SExtractor: Handle fully correlated x/y (which cause a singularity...)
  SeFloat tmp = x_2 * y_2 - x_y * x_y;
  if (tmp < 0.00694) {
    x_2 += 0.0833333;
    y_2 += 0.0833333;
    tmp = x_2 * y_2 - x_y * x_y;

    if ((x_2_error*y_2_error-x_y_error*x_y_error)  <  total_variance*total_variance) {
    	x_2_error += total_variance;
    	y_2_error += total_variance;
    }

  }

  SeFloat cxx = y_2 / tmp;
  SeFloat cyy = x_2 / tmp;
  SeFloat cxy = -2.0 * x_y / tmp;

  SeFloat area_under_half = (double) pixel_values.size() - nb_of_pixels_above_half;
  SeFloat t1t2 = min_value / half_peak_threshold;

  //std::cout << "a "<< a << " b " << b << " t1t2 " << t1t2 << " area " << area_under_half << "     " << min_value << " " << half_peak_threshold << std::endl;

  SeFloat abcor = 1.0;
  if (t1t2 > 0.0) { // && !prefs.dweight_flag
    abcor = (area_under_half > 0.0 ? area_under_half : 1.0) / (2 * M_PI * -log(t1t2 < 1.0 ? t1t2 : 0.99) * a * b);
    if (abcor > 1.0) {
      abcor = 1.0;
    }
  }

  /*------------------------- Error ellipse parameters ------------------------*/
  /*    if (FLAG(obj2.poserr_a))
        {
         double	pmx2,pmy2,temp,theta;

        if (fabs(temp=obj->poserr_mx2-obj->poserr_my2) > 0.0)
          theta = atan2(2.0 * obj->poserr_mxy,temp) / 2.0;
        else
          theta = PI/4.0;

        temp = sqrt(0.25*temp*temp+obj->poserr_mxy*obj->poserr_mxy);
        pmy2 = pmx2 = 0.5*(obj->poserr_mx2+obj->poserr_my2);
        pmx2+=temp;
        pmy2-=temp;

        obj2->poserr_a = (float)sqrt(pmx2);
        obj2->poserr_b = (float)sqrt(pmy2);
        obj2->poserr_theta = theta*180.0/PI;
        }
	*/

  // error for theta_image
  SeFloat theta_error, a_error, b_error;
  SeFloat temp_error = fabs(x_2_error - y_2_error);
  if (temp_error>0.0)
	  //theta_error = atan2(2.0*x_y_error, temp_error) /  2.0;
	  theta_error = atan2(2.0*x_y_error, temp_error);
  else
	  theta_error = M_PI /4.0;

  // error for a/b_image
  temp_error = sqrt(0.25*temp_error*temp_error+x_y_error*x_y_error);
  a_error = 0.5*(x_2_error+y_2_error);
  b_error = a_error;
  a_error = sqrt(a_error+temp_error);
  b_error = sqrt(b_error-temp_error);

  source.setProperty<ShapeParameters>(a, b, theta, a_error, b_error, theta_error, abcor, cxx, cyy, cxy, x_2_error, y_2_error, x_y_error, nb_of_pixels);
}


}

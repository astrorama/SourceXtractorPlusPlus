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

#include <iostream>

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
#include "SEImplementation/Plugin/ErrorEllipse/ErrorEllipse.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/PeakValue/PeakValue.h"

#include "SEImplementation/Plugin/ErrorEllipse/ErrorEllipseTask.h"

namespace SourceXtractor {

void ErrorEllipseTask::computeProperties(SourceInterface& source) const {
  const auto& pixel_values = source.getProperty<DetectionFramePixelValues>().getFilteredValues();
  const auto& pixel_variances = source.getProperty<DetectionFramePixelValues>().getVariances();
  const auto& centroid_x = source.getProperty<PixelCentroid>().getCentroidX();
  const auto& centroid_y = source.getProperty<PixelCentroid>().getCentroidY();
  const auto& coordinates = source.getProperty<PixelCoordinateList>().getCoordinateList();
  auto total_intensity = source.getProperty<ShapeParameters>().getIntensity();
  auto singu = source.getProperty<ShapeParameters>().getSinguFlag();

  SeFloat x_2_error = 0.0;
  SeFloat y_2_error = 0.0;
  SeFloat x_y_error = 0.0;

  //SeFloat total_intensity = 0;
  SeFloat total_variance = 0;

  auto j = pixel_variances.begin();
  //auto i = pixel_values.begin();
  for (auto pixel_coord : coordinates) {
    //SeFloat value = *i++;
	SeFloat variance = *j++;
	SeFloat x_pos = pixel_coord.m_x - centroid_x;
	SeFloat y_pos = pixel_coord.m_y - centroid_y;

    x_2_error += x_pos * x_pos * variance;
    y_2_error += y_pos * y_pos * variance;
    x_y_error += x_pos * y_pos * variance;

    //total_intensity += value;
    total_variance  += variance;
  }


  SeFloat total_intensity_2 = total_intensity * total_intensity;
  x_2_error /= total_intensity_2;
  y_2_error /= total_intensity_2;
  x_y_error /= total_intensity_2;
  //esum *= 0.08333/flux2;
  total_variance *= 0.08333/total_intensity_2;

  if (singu && (x_2_error*y_2_error-x_y_error*x_y_error)<total_variance*total_variance) {
  	x_2_error += total_variance;
  	y_2_error += total_variance;
  	//std::cout << " singuflag " << std::endl;
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

  // ellipse error for theta_image
  SeFloat theta_error, a_error, b_error;
  SeFloat temp_error = fabs(x_2_error - y_2_error);
  if (temp_error>0.0)
	  //theta_error = atan2(2.0*x_y_error, temp_error) /  2.0;
	  theta_error = atan2(2.0*x_y_error, temp_error); // when dividing by 2.0 as in SE2 the range is -45 < theta_error < 45 which is smaller than in SE2
  else
	  theta_error = M_PI /4.0;

  // error for a/b_image
  temp_error = sqrt(0.25*temp_error*temp_error+x_y_error*x_y_error);
  a_error = 0.5*(x_2_error+y_2_error);
  b_error = a_error;
  a_error = sqrt(a_error+temp_error);
  b_error = sqrt(b_error-temp_error);

  source.setProperty<ErrorEllipse>(a_error, b_error, theta_error, x_2_error, y_2_error, x_y_error);
}


}

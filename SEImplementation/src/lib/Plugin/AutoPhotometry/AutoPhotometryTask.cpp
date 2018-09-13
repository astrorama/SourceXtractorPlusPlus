/*
 * AutoPhotometryTask.cpp
 *
 *  Created on: Jul 18, 2016
 *      Author: mkuemmel@usm.lmu.de
 */
//#include <math.h>
#include <iostream>

#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Plugin/PixelCentroid/PixelCentroid.h"
#include "SEImplementation/Plugin/ShapeParameters/ShapeParameters.h"
#include "SEImplementation/Plugin/KronRadius/KronRadius.h"

#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometry.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryTask.h"

namespace SExtractor {

namespace {
  // enhancing from 5 to 10 smoothens the photometry
  const int SUPERSAMPLE_AUTO_NB = 10;
}

//////////////////////////////////////////////////////////////////////////////////////////

void AutoPhotometryTask::computeProperties(SourceInterface& source) const {
  // get the detection frame
  const auto& detection_frame  = source.getProperty<DetectionFrame>().getFrame();

  // get the images and image information from the frame
  const auto& detection_image    = detection_frame->getSubtractedImage();
  const auto& detection_variance = detection_frame->getVarianceMap();
  const auto& variance_threshold = detection_frame->getVarianceThreshold();

  // get the object center
  const auto& centroid_x = source.getProperty<PixelCentroid>().getCentroidX();
  const auto& centroid_y = source.getProperty<PixelCentroid>().getCentroidY();

  // get the shape parameters
  const auto& cxx = source.getProperty<ShapeParameters>().getEllipseCxx();
  const auto& cyy = source.getProperty<ShapeParameters>().getEllipseCyy();
  const auto& cxy = source.getProperty<ShapeParameters>().getEllipseCxy();
  const auto& ell_a = source.getProperty<ShapeParameters>().getEllipseA();
  const auto& ell_b = source.getProperty<ShapeParameters>().getEllipseB();

  // get the kron-radius
  //SeFloat kron_radius_auto = m_kron_factor*source.getProperty<KronRadius>().getKronRadius();
  //if (kron_radius_auto < m_kron_minrad)
  //  kron_radius_auto = m_kron_minrad;
  //SeFloat kron_radius_auto = 2.5*source.getProperty<KronRadius>().getKronRadius();
  //if (kron_radius_auto < 3.5)
  //  kron_radius_auto = 3.5;
  auto kron_radius_auto = source.getProperty<KronRadius>().getKronRadius();

  /*
  if (r1>0.0 && v1>0.0)
    {
    obj2->kronfactor = prefs.autoparam[0]*r1/v1;
    if (obj2->kronfactor < prefs.autoparam[1])
      obj2->kronfactor = prefs.autoparam[1];
    }
  else
    obj2->kronfactor = prefs.autoparam[1];
  */

  // create the elliptical aperture
  //auto ell_aper = std::make_shared<EllipticalAperture>(centroid_x, centroid_y, cxx, cyy, cxy, m_kron_factor);
  auto ell_aper = std::make_shared<EllipticalAperture>(centroid_x, centroid_y, cxx, cyy, cxy, kron_radius_auto);

  // get the aperture borders on the image
  const auto& min_pixel = ell_aper->getMinPixel();
  const auto& max_pixel = ell_aper->getMaxPixel();

  SeFloat total_flux     = 0;
  SeFloat total_variance = 0.0;
  //SeFloat total_area = 0.0;

  // iterate over the aperture pixels
  for (int pixel_y = min_pixel.m_y; pixel_y <= max_pixel.m_y; pixel_y++) {
    for (int pixel_x = min_pixel.m_x; pixel_x <= max_pixel.m_x; pixel_x++) {
      SeFloat value = 0;
      SeFloat pixel_variance = 0;

      // make sure the pixel is inside the image
      if (pixel_x >=0 && pixel_y >=0 && pixel_x < detection_image->getWidth() && pixel_y < detection_image->getHeight()) {

        // get the variance value
        pixel_variance = detection_variance ? detection_variance->getValue(pixel_x, pixel_y) : 1;

        // check whether the pixel is "good"
        if (pixel_variance < variance_threshold) {
          value = detection_image->getValue(pixel_x, pixel_y);
        }
        else if (m_use_symmetry) {

          // for bad pixels get the mirror pixel
          auto mirror_x = 2 * centroid_x - pixel_x + 0.49999;
          auto mirror_y = 2 * centroid_y - pixel_y + 0.49999;
          if (mirror_x >=0 && mirror_y >=0 && mirror_x < detection_image->getWidth() && mirror_y < detection_image->getHeight()) {
            pixel_variance = detection_variance ? detection_variance->getValue(mirror_x, mirror_y) : 1;

            // check whether the mirror pixel is good
            if (pixel_variance < variance_threshold) {
              value = detection_image->getValue(mirror_x, mirror_y);
            } else {
              // set bad pixel values to zero
              value=0.0;
              pixel_variance=0.0;
            }
          }
        } else {
          // set bad pixel values to zero
          value=0.0;
          pixel_variance=0.0;
        }
        // get the area and enhance flux and variance
        auto area = ell_aper->getArea(pixel_x, pixel_y);
        total_flux     += value * area;
        total_variance += pixel_variance * area;
        if (area > 0.0){
          m_tmp_check_image->setValue(pixel_x, pixel_y, m_tmp_check_image->getValue(pixel_x, pixel_y)+area);
        }
      }
    }
  }

  // compute the derived quantities
  auto flux_error = sqrt(total_variance);
  auto mag = total_flux > 0.0 ? -2.5*log10(total_flux) + m_magnitude_zero_point : SeFloat(99.0);
  auto mag_error = 1.0857 * flux_error / total_flux;

  // set the source properties
  source.setProperty<AutoPhotometry>(total_flux, flux_error, mag, mag_error);
}

//////////////////////////////////////////////////////////////////////////////////////////
/*
void AutoPhotometryAggregateTask::computeProperties(SourceInterface& source) const {
  SeFloat flux = 0;
  for (auto instance : m_instances_to_aggregate) {
    auto aperture_photometry = source.getProperty<AperturePhotometry>(instance);
    flux += aperture_photometry.getFlux();
  }
  flux /= m_instances_to_aggregate.size();

  auto mag = flux > 0.0 ? -2.5*log10(flux) + m_magnitude_zero_point : SeFloat(99.0);
  source.setIndexedProperty<AperturePhotometry>(m_instance, flux, 999999, mag, 999999); // FIXME
}
*/

//////////////////////////////////////////////////////////////////////////////////////////


SeFloat EllipticalAperture::getAreaSub(int pixel_x, int pixel_y) const{
  SeFloat act_x_prim, act_y_prim;
  SeFloat act_x, act_y;

  //std::cout << " quest for:" << pixel_x << ":" << pixel_y << " ? " << m_rad_max <<std::endl;

  // set the start value in y
  if (SUPERSAMPLE_AUTO_NB % 2)
    // for odd sub-samples
    act_y = -1.0/SUPERSAMPLE_AUTO_NB * (SUPERSAMPLE_AUTO_NB/2);
  else
    // for even sub-samples
    act_y = 1./(2*SUPERSAMPLE_AUTO_NB) - (1./SUPERSAMPLE_AUTO_NB)*(SUPERSAMPLE_AUTO_NB/2);

  // iterate over all sub-samples in y
  SeFloat area = 0.0;
  for (int sub_y = 0; sub_y < SUPERSAMPLE_AUTO_NB; sub_y++) {

    // set the start value in x
    if (SUPERSAMPLE_AUTO_NB % 2)
      // for odd sub-samples
      act_x = -1.0/SUPERSAMPLE_AUTO_NB * (SUPERSAMPLE_AUTO_NB/2);
    else
      // for even sub-samples
      act_x = 1./(2*SUPERSAMPLE_AUTO_NB) - (1./SUPERSAMPLE_AUTO_NB)*(SUPERSAMPLE_AUTO_NB/2);

    // iterate over all sub-samples in x
    for (int sub_x = 0; sub_x < SUPERSAMPLE_AUTO_NB; sub_x++) {
      //std::cout << " spix:" << act_x << ":" << act_y;
      // CXX(x − x) 2 + CYY(y − y) 2 + CXY(x − x)(y − y) = R 2
      auto dist_x = SeFloat(pixel_x)+act_x-m_center_x;
      auto dist_y = SeFloat(pixel_y)+act_y-m_center_y;
      //if (m_cxx*(SeFloat(pixel_x)+act_x-m_center_x)*(SeFloat(pixel_x)+act_x-m_center_x) + m_cyy*(SeFloat(pixel_y)+act_y-m_center_y)*(SeFloat(pixel_y)+act_y-m_center_y) + m_cxy*(SeFloat(pixel_y)+act_x-m_center_x)*(SeFloat(pixel_y)+act_y-m_center_y)<1.0)
      if (m_cxx*dist_x*dist_x + m_cyy*dist_y*dist_y + m_cxy*dist_x*dist_y < m_rad_max*m_rad_max)
      // enhance the are if the sub-pixel is in
        area += 1.0 / (SUPERSAMPLE_AUTO_NB * SUPERSAMPLE_AUTO_NB);
      //}
      // increment in x
      act_x += 1./SUPERSAMPLE_AUTO_NB;
    }
    // increment in y
    act_y += 1./SUPERSAMPLE_AUTO_NB;
  }
  //std::cout << " area:" << area;
  // return the area
  return area;
}

SeFloat EllipticalAperture::getArea(int pixel_x, int pixel_y) const{
  auto dist_x = SeFloat(pixel_x)-m_center_x;
  auto dist_y = SeFloat(pixel_y)-m_center_y;

  if (m_cxx*dist_x*dist_x + m_cyy*dist_y*dist_y + m_cxy*dist_x*dist_y < m_rad_max*m_rad_max)
    return 1.0;
  else
    return 0.0;
}

SeFloat EllipticalAperture::getRadiusSquared(int pixel_x, int pixel_y) const{
  auto dist_x = SeFloat(pixel_x)-m_center_x;
  auto dist_y = SeFloat(pixel_y)-m_center_y;

  return m_cxx*dist_x*dist_x + m_cyy*dist_y*dist_y + m_cxy*dist_x*dist_y;
}

PixelCoordinate EllipticalAperture::getMinPixel() const {
  SeFloat dx, dy;

  // compute the maximum extend in x/y
  dx = m_rad_max*std::sqrt(1.0/(m_cxx - m_cxy*m_cxy/(4.0*m_cxx)));
  dy = m_rad_max*std::sqrt(1.0/(m_cyy - m_cxy*m_cxy/(4.0*m_cyy)));

  // return the absolute values
  return PixelCoordinate(m_center_x - dx,  m_center_y - dy);
}

PixelCoordinate EllipticalAperture::getMaxPixel() const {
  SeFloat dx, dy;

  // compute the maximum extend in x/y
  dx = m_rad_max*std::sqrt(1.0/(m_cxx - m_cxy*m_cxy/(4.0*m_cxx)));
  dy = m_rad_max*std::sqrt(1.0/(m_cyy - m_cxy*m_cxy/(4.0*m_cyy)));

  // return the absolute values
  return PixelCoordinate(m_center_x + dx + 1,  m_center_y + dy + 1);
}

}


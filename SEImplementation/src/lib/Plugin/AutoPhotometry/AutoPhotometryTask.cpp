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

  const auto& cxx = source.getProperty<ShapeParameters>().getEllipseCxx();
  const auto& cyy = source.getProperty<ShapeParameters>().getEllipseCyy();
  const auto& cxy = source.getProperty<ShapeParameters>().getEllipseCxy();
  const auto& ell_a = source.getProperty<ShapeParameters>().getEllipseA();
  const auto& ell_b = source.getProperty<ShapeParameters>().getEllipseB();
  auto ell_aper = std::make_shared<RotatedEllipticalApertureAlt>(centroid_x, centroid_y, cxx, cyy, cxy, m_kron_factor);
  //auto ell_aper = std::make_shared<RotatedEllipticalApertureAlt>(centroid_x, centroid_y, cxx, cyy, cxy, 3.0);
  //auto ell_aper = std::make_shared<RotatedEllipticalApertureAlt>(centroid_x, centroid_y, cxx, cyy, cxy, 1.0);


  /*
  // get the shape parameters
  SeFloat ell_a     = m_kron_factor*source.getProperty<ShapeParameters>().getEllipseA();
  ell_a = ell_a < m_kron_minrad ? m_kron_minrad : ell_a;
  SeFloat ell_b     = m_kron_factor*source.getProperty<ShapeParameters>().getEllipseB();
  ell_b = ell_b < m_kron_minrad ? m_kron_minrad : ell_b;
  const auto& ell_theta = source.getProperty<ShapeParameters>().getEllipseTheta();
  //std::cout << ell_a << " " << ell_b << std::endl;
  // create the aperture
  auto ell_aper = std::make_shared<RotatedEllipticalAperture>(centroid_x, centroid_y, ell_theta, ell_a, ell_b);
  */

  // get the aperture borders on the image
  PixelCoordinate min_pixel;
  PixelCoordinate max_pixel;
  ell_aper->getMinMaxPixel(min_pixel, max_pixel);

  //std::cout << " " << min_pixel.m_x << "," << max_pixel.m_x << " : " << min_pixel.m_y << "," << max_pixel.m_y << " ? " << max_pixel.m_x-min_pixel.m_x << "," << max_pixel.m_y-min_pixel.m_y << std::endl;
  //std::cout << " center:" << centroid_x << "," << centroid_y << " x/ylimits:" << min_pixel.m_x << "," << max_pixel.m_x << " : " << min_pixel.m_y << "," << max_pixel.m_y << " ? " << max_pixel.m_x-min_pixel.m_x << "," << max_pixel.m_y-min_pixel.m_y << std::endl;

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

SeFloat EllipticalAperture::getArea(int pixel_x, int pixel_y) const {
  return getArea(SeFloat(pixel_x), SeFloat(pixel_y));
}

SeFloat EllipticalAperture::getArea(SeFloat pixel_x, SeFloat pixel_y) const {

  auto ell_distance = (pixel_x*pixel_x)/(m_major_axis*m_major_axis) + (pixel_y*pixel_y)/(m_minor_axis*m_minor_axis);
  if (ell_distance < 1.0)
    return 1.0;
  else
    return 0.0;


  /*
  SeFloat min_supersampled_radius_squared = m_radius > .75 ? (m_radius - .75) * (m_radius - .75) : 0;
  SeFloat max_supersampled_radius_squared = (m_radius + .75) * (m_radius + .75);
  auto distance_squared = dx * dx + dy * dy;
  if (distance_squared <= max_supersampled_radius_squared) {
    if (distance_squared < min_supersampled_radius_squared) {
      return 1.0;
    } else {
      SeFloat area = 0.0;
      for (int sub_y = 0; sub_y < SUPERSAMPLE_AUTO_NB; sub_y++) {
        for (int sub_x = 0; sub_x < SUPERSAMPLE_AUTO_NB; sub_x++) {
          auto dx2 = dx + SeFloat(sub_x - SUPERSAMPLE_AUTO_NB/2) / SUPERSAMPLE_AUTO_NB;
          auto dy2 = dy + SeFloat(sub_y - SUPERSAMPLE_AUTO_NB/2) / SUPERSAMPLE_AUTO_NB;
          auto supersampled_distance_squared = dx2 * dx2 + dy2 * dy2;
          if (supersampled_distance_squared <= m_radius * m_radius) {
            area += 1.0 / (SUPERSAMPLE_AUTO_NB * SUPERSAMPLE_AUTO_NB);
          }
        }
      }
      return area;
    }
  }
  return 0.0;
  */
}

PixelCoordinate EllipticalAperture::getMinPixel() const {
  return PixelCoordinate(-1.0*m_major_axis, -1.0*m_minor_axis);
}

PixelCoordinate EllipticalAperture::getMaxPixel() const {
  return PixelCoordinate(m_major_axis + 1, m_minor_axis + 1);
}

SeFloat RotatedEllipticalAperture::getArea(int pixel_x, int pixel_y) const{
  SeFloat act_x_prim, act_y_prim;
  SeFloat act_x, act_y;

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

      // rotate into the ellipse
      act_x_prim =  m_cos*(SeFloat(pixel_x) + act_x-m_center_x) + m_sin*(SeFloat(pixel_y) + act_y-m_center_y);
      act_y_prim = -m_sin*(SeFloat(pixel_x) + act_x-m_center_x) + m_cos*(SeFloat(pixel_y) + act_y-m_center_y);

      // enhance the are if the sub-pixel is in
      if (ell_aper->getArea(act_x_prim, act_y_prim)>0.) {
        area += 1.0 / (SUPERSAMPLE_AUTO_NB * SUPERSAMPLE_AUTO_NB);
      }
      // increment in x
      act_x += 1./SUPERSAMPLE_AUTO_NB;
    }
    // increment in y
    act_y += 1./SUPERSAMPLE_AUTO_NB;
  }
  // return the area
  return area;
}

PixelCoordinate RotatedEllipticalAperture::getMinPixel() const {
  SeFloat dx, dy;

  // get the min of the basic ellipse
  auto ell_min = ell_aper->getMinPixel();

  // rotate from the basic ellipse into the image
  dx =  m_cos*SeFloat(ell_min.m_x) + m_sin*SeFloat(ell_min.m_y);
  dy = -m_sin*SeFloat(ell_min.m_x) + m_cos*SeFloat(ell_min.m_y);

  // return the absolute values
  return PixelCoordinate(m_center_x + dx,  m_center_y + dy);
}

PixelCoordinate RotatedEllipticalAperture::getMaxPixel() const {
  SeFloat dx, dy;

  // get the min of the basic ellipse
  auto ell_max = ell_aper->getMaxPixel();

  // rotate from the basic ellipse into the image
  dx =  m_cos*SeFloat(ell_max.m_x) + m_sin*SeFloat(ell_max.m_y);
  dy = -m_sin*SeFloat(ell_max.m_x) + m_cos*SeFloat(ell_max.m_y);

  // return the absolute values
  return PixelCoordinate(m_center_x + dx + 1,  m_center_y + dy + 1);
}


void RotatedEllipticalAperture::getMinMaxPixel(PixelCoordinate& min, PixelCoordinate& max){
  SeFloat dx1, dy1, dx2, dy2;

  // get the min and max of the basic ellipse
  auto ell_min = ell_aper->getMinPixel();
  auto ell_max = ell_aper->getMaxPixel();
  //std::cout << " coo1: " << ell_min.m_x << "," << ell_min.m_y << " coo2: " << ell_max.m_x << "," << ell_max.m_y << std::endl;

  // that's very rough approximation: use the major axis for all measures
  min.m_x = m_center_x + ell_min.m_x;
  min.m_y = m_center_y + ell_min.m_x ;

  max.m_x = m_center_x + ell_max.m_x + 1;
  max.m_y = m_center_y + ell_max.m_x + 1;

  /*
  // rotate from the basic ellipse into the image
  dx1 =  m_cos*SeFloat(ell_min.m_x) + m_sin*SeFloat(ell_min.m_y);
  dy1 = -m_sin*SeFloat(ell_min.m_x) + m_cos*SeFloat(ell_min.m_y);

  // rotate from the basic ellipse into the image
  dx2 =  m_cos*SeFloat(ell_max.m_x) + m_sin*SeFloat(ell_max.m_y);
  dy2 = -m_sin*SeFloat(ell_max.m_x) + m_cos*SeFloat(ell_max.m_y);

  std::cout << " dxy1: " << dx1 << "," << dy1 << " dxy2: " << dx2 << "," << dy2 << " ? cos: "<< m_cos << " sin: "<< m_sin << " rot: " << m_rotation_angle <<std::endl;

  if (dx1 < dx2){
    min.m_x = m_center_x + dx1;
    max.m_x = m_center_x + dx2 + 1;
  }
  else{
    min.m_x = m_center_x + dx2;
    max.m_x = m_center_x + dx1 + 1;
  }

  if (dy1 < dy2){
    min.m_y = m_center_y + dy1;
    max.m_y = m_center_y + dy2 + 1;
  }
  else{
    min.m_y = m_center_y + dy2;
    max.m_y = m_center_y + dy1 + 1;
  }
   */
}

SeFloat RotatedEllipticalApertureAlt::getArea(int pixel_x, int pixel_y) const{
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
  // return the area
  return area;
}

PixelCoordinate RotatedEllipticalApertureAlt::getMinPixel() const {
  SeFloat dx, dy;

  // compute the maximum extend in x/y
  dx = m_rad_max*std::sqrt(1.0/(m_cxx - m_cxy*m_cxy/(4.0*m_cxx)));
  dy = m_rad_max*std::sqrt(1.0/(m_cyy - m_cxy*m_cxy/(4.0*m_cyy)));

  // return the absolute values
  return PixelCoordinate(m_center_x - dx,  m_center_y - dy);
}

PixelCoordinate RotatedEllipticalApertureAlt::getMaxPixel() const {
  SeFloat dx, dy;

  // compute the maximum extend in x/y
  dx = m_rad_max*std::sqrt(1.0/(m_cxx - m_cxy*m_cxy/(4.0*m_cxx)));
  dy = m_rad_max*std::sqrt(1.0/(m_cyy - m_cxy*m_cxy/(4.0*m_cyy)));

  // return the absolute values
  return PixelCoordinate(m_center_x + dx + 1,  m_center_y + dy + 1);
}


void RotatedEllipticalApertureAlt::getMinMaxPixel(PixelCoordinate& min, PixelCoordinate& max){
  SeFloat dx, dy;

  // compute the maximum extend in x/y
  //dx = m_rad_max*std::sqrt(1.0/(m_cxx - m_cxy*m_cxy/(4.0*m_cxx)));
  //dy = m_rad_max*std::sqrt(1.0/(m_cyy - m_cxy*m_cxy/(4.0*m_cyy)));

  // insert the minimum values
  // min.m_x = m_center_x - dx;
  // min.m_y = m_center_y - dy ;

  // insert the maximum values
  // max.m_x = m_center_x + dx + 1;
  // max.m_y = m_center_y + dy + 1;

  // get the min and the max pixels
  auto min_pix = getMinPixel();
  auto max_pix = getMaxPixel();

  // transfer the values
  min.m_x = min_pix.m_x;
  min.m_y = min_pix.m_y;
  max.m_x = max_pix.m_x;
  max.m_y = max_pix.m_y;
}


}


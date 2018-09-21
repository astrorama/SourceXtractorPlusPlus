/*
 * AperturePhotometryTask.h
 *
 *  Created on: Sep 22, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYTASK_H_

#include "SEUtils/PixelCoordinate.h"
#include "SEFramework/Task/SourceTask.h"
#include "SEFramework/Image/VectorImage.h"

namespace SExtractor {

class Aperture {
public:
  virtual ~Aperture() = default;
  virtual SeFloat getArea(SeFloat center_x, SeFloat center_y, int pixel_x, int pixel_y) const = 0;
  virtual PixelCoordinate getMinPixel(SeFloat centroid_x, SeFloat centroid_y) const = 0;
  virtual PixelCoordinate getMaxPixel(SeFloat centroid_x, SeFloat centroid_y) const = 0;
};

class CircularAperture : public Aperture {
public:
  virtual ~CircularAperture() = default;
  CircularAperture(SeFloat radius) : m_radius(radius) {}

  virtual SeFloat getArea(SeFloat center_x, SeFloat center_y, int pixel_x, int pixel_y) const override;
  virtual PixelCoordinate getMinPixel(SeFloat centroid_x, SeFloat centroid_y) const override;
  virtual PixelCoordinate getMaxPixel(SeFloat centroid_x, SeFloat centroid_y) const override;

private:
  SeFloat m_radius;
};

class NeighbourInfo{
public:
  /// Destructor
  virtual ~NeighbourInfo() = default;

  NeighbourInfo(const PixelCoordinate& min_pixel, const PixelCoordinate& max_pixel, const std::vector<PixelCoordinate>& pixel_list, const std::shared_ptr<Image<SeFloat>> threshold_image)
  : m_offset(min_pixel) {

    // create the image
    auto x_size=max_pixel.m_x-min_pixel.m_x+1;
    auto y_size=max_pixel.m_y-min_pixel.m_y+1;
    neighbour_image = VectorImage<int>::create(x_size, y_size, std::vector<int>(x_size*y_size, 0));

    //std::cout << "width: " << neighbour_image->getWidth() << ", height: " << neighbour_image->getHeight() << std::endl;

    // set the pixels belonging to the source to -1
    for (auto pixel_coord : pixel_list) {
      auto act_x = pixel_coord.m_x-m_offset.m_x;
      auto act_y = pixel_coord.m_y-m_offset.m_y;

      // make sure to be inside the image
      if (act_x>=0 && act_y>=0 && act_x<neighbour_image->getWidth() && act_y<neighbour_image->getHeight())
        neighbour_image->setValue(act_x, act_y, -1);
    }

    // go over the image
    for (int act_y=0; act_y<neighbour_image->getHeight(); act_y++) {
      for (int act_x=0; act_x<neighbour_image->getWidth(); act_x++) {
        int offset_x = act_x+m_offset.m_x;
        int offset_y = act_y+m_offset.m_y;

        // set surrounding pixels above the threshold to 1, all others to 0
        if (offset_x>=0 && offset_y>=0 && offset_x<threshold_image->getWidth() && offset_y<threshold_image->getHeight()) {
          if (threshold_image->getValue(offset_x, offset_y)>0){
            if (neighbour_image->getValue(act_x, act_y)!=-1)
              neighbour_image->setValue(act_x, act_y, 1);
            else
              neighbour_image->setValue(act_x, act_y, 0);
          }
          else
            neighbour_image->setValue(act_x, act_y, 0);
        }
      }
    }
  }

  int isNeighbourObjectPixel(int x, int y ){
    auto act_x=x-m_offset.m_x;
    auto act_y=y-m_offset.m_y;
    assert(act_x>=0 && act_y>=0 && act_x<neighbour_image->getWidth() && act_y<neighbour_image->getHeight());
    return neighbour_image->getValue(act_x, act_y);
  }

private:
  std::shared_ptr<VectorImage<int>> neighbour_image;

  PixelCoordinate m_offset;
};

class AperturePhotometryTask : public SourceTask {
public:

  using AreaFunction = std::function<SeFloat(int, int)>;

  /// Destructor
  virtual ~AperturePhotometryTask() = default;

  AperturePhotometryTask(std::shared_ptr<Aperture> aperture, unsigned int instance, unsigned int image_instance,
      SeFloat magnitude_zero_point, bool use_symmetry)
      : m_aperture(aperture),
        m_instance(instance),
        m_image_instance(image_instance),
        m_magnitude_zero_point(magnitude_zero_point),
        m_use_symmetry(use_symmetry) {}

  virtual void computeProperties(SourceInterface& source) const override;

private:
  std::shared_ptr<Aperture> m_aperture;
  unsigned int m_instance;
  unsigned int m_image_instance;
  SeFloat m_magnitude_zero_point;
  bool m_use_symmetry;

};

class AperturePhotometryAggregateTask : public SourceTask {
public:

  AperturePhotometryAggregateTask(
    unsigned int instance, std::vector<unsigned int> instances_to_aggregate, SeFloat magnitude_zero_point
  ) :
      m_instance(instance),
      m_instances_to_aggregate(instances_to_aggregate),
      m_magnitude_zero_point(magnitude_zero_point) {}

  virtual ~AperturePhotometryAggregateTask() = default;

  virtual void computeProperties(SourceInterface& source) const override;

private:
  unsigned int m_instance;
  std::vector<unsigned int> m_instances_to_aggregate;
  SeFloat m_magnitude_zero_point;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYTASK_H_ */

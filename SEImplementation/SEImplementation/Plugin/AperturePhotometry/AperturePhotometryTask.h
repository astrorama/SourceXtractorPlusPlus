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

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

  /**
   * @brief Destructor
   */
  virtual ~AperturePhotometryTask() = default;

  AperturePhotometryTask(std::shared_ptr<Aperture> aperture, unsigned int instance, SeFloat magnitude_zero_point)
      : m_aperture(aperture),
        m_instance(instance),
        m_magnitude_zero_point(magnitude_zero_point) {}

  virtual void computeProperties(SourceInterface& source) const override;

private:
  SeFloat m_magnitude_zero_point;
  unsigned int m_instance;

  std::shared_ptr<Aperture> m_aperture;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYTASK_H_ */

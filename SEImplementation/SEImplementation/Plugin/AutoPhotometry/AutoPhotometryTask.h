/*
 * AperturePhotometryTask.h
 *
 *  Created on: Sep 22, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYTASK_H_

//#include <memory>

#include "SEUtils/PixelCoordinate.h"
#include "SEFramework/Task/SourceTask.h"

namespace SExtractor {

class MAperture {
public:
  virtual ~MAperture() = default;
  virtual SeFloat getArea(int pixel_x, int pixel_y) const = 0;
  virtual PixelCoordinate getMinPixel() const = 0;
  virtual PixelCoordinate getMaxPixel() const = 0;
};

class EllipticalAperture : public MAperture {
public:
  virtual ~EllipticalAperture() = default;
  EllipticalAperture(SeFloat major_axis, SeFloat minor_axis) :
    m_major_axis(major_axis),
    m_minor_axis(minor_axis) {}

  virtual SeFloat getArea(SeFloat pixel_x, SeFloat pixel_y) const;
  virtual SeFloat getArea(int pixel_x, int pixel_y) const override;
  virtual PixelCoordinate getMinPixel() const override;
  virtual PixelCoordinate getMaxPixel() const override;

private:
  SeFloat m_major_axis;
  SeFloat m_minor_axis;
};

class RotatedEllipticalAperture : public MAperture {
public:
  virtual ~RotatedEllipticalAperture() = default;
  RotatedEllipticalAperture(SeFloat center_x, SeFloat center_y, SeFloat rotation_angle, SeFloat major_axis, SeFloat minor_axis) :
    m_center_x(center_x),
    m_center_y(center_y),
    m_rotation_angle(rotation_angle),
    m_sin(std::sin(rotation_angle)),
    m_cos(std::cos(rotation_angle))
  {
    ell_aper = std::make_shared<EllipticalAperture>(major_axis, minor_axis);
  }

  virtual SeFloat getArea(int pixel_x, int pixel_y) const override;
  virtual PixelCoordinate getMinPixel() const override;
  virtual PixelCoordinate getMaxPixel() const override;
  virtual void getMinMaxPixel(PixelCoordinate& min, PixelCoordinate& max);

private:
  std::shared_ptr<EllipticalAperture> ell_aper;
  SeFloat m_rotation_angle;
  SeFloat m_center_x;
  SeFloat m_center_y;
  SeFloat m_sin;
  SeFloat m_cos;
};


class AutoPhotometryTask : public SourceTask {
public:

  //using AreaFunction = std::function<SeFloat(int, int)>;

  /// Destructor
  virtual ~AutoPhotometryTask() = default;

  AutoPhotometryTask(SeFloat magnitude_zero_point, SeFloat kron_factor,  SeFloat kron_minrad,  SeFloat kron_estimation,  SeFloat kron_measurement, bool use_symmetry) :
    m_magnitude_zero_point(magnitude_zero_point),
    m_kron_factor(kron_factor),
    m_kron_minrad(kron_minrad),
    m_kron_estimation(kron_estimation),
    m_kron_measurement(kron_measurement),
    m_use_symmetry(use_symmetry) {}

  virtual void computeProperties(SourceInterface& source) const override;

private:
  SeFloat m_magnitude_zero_point;
  SeFloat m_kron_factor;
  SeFloat m_kron_minrad;
  SeFloat m_kron_estimation;
  SeFloat m_kron_measurement;
  bool m_use_symmetry;
};

/*
class AutoPhotometryAggregateTask : public SourceTask {
public:

  AutoPhotometryAggregateTask(
    unsigned int instance, std::vector<unsigned int> instances_to_aggregate, SeFloat magnitude_zero_point
  ) :
      m_instance(instance),
      m_instances_to_aggregate(instances_to_aggregate),
      m_magnitude_zero_point(magnitude_zero_point) {}

  virtual ~AutoPhotometryAggregateTask() = default;

  virtual void computeProperties(SourceInterface& source) const override;

private:
  unsigned int m_instance;
  std::vector<unsigned int> m_instances_to_aggregate;
  SeFloat m_magnitude_zero_point;
};
*/
}

#endif /* _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYTASK_H_ */

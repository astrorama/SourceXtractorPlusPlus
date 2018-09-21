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

class AutoPhotometryTask : public SourceTask {
public:

  //using AreaFunction = std::function<SeFloat(int, int)>;

  /// Destructor
  virtual ~AutoPhotometryTask() = default;
  /*
  AutoPhotometryTask(SeFloat magnitude_zero_point, SeFloat kron_factor, SeFloat kron_minrad, bool use_symmetry, std::shared_ptr<WriteableImage<float>> tmp_check_image) :
    m_magnitude_zero_point(magnitude_zero_point),
    m_kron_factor(kron_factor),
    m_kron_minrad(kron_minrad),
    m_use_symmetry(use_symmetry),
    m_tmp_check_image(tmp_check_image) {}
*/

  AutoPhotometryTask(SeFloat magnitude_zero_point, SeFloat kron_factor, SeFloat kron_minrad, bool use_symmetry) :
    m_magnitude_zero_point(magnitude_zero_point),
    m_kron_factor(kron_factor),
    m_kron_minrad(kron_minrad),
    m_use_symmetry(use_symmetry) {}

  virtual void computeProperties(SourceInterface& source) const override;

private:
  SeFloat m_magnitude_zero_point;
  SeFloat m_kron_factor;
  SeFloat m_kron_minrad;
  bool m_use_symmetry;

  // TEMP
  //std::shared_ptr<WriteableImage<float>> m_tmp_check_image;
};

class EAperture {
public:
  virtual ~EAperture() = default;
  virtual SeFloat getAreaSub(int pixel_x, int pixel_y) const = 0;
  virtual int getArea(int pixel_x, int pixel_y) const = 0;
  virtual PixelCoordinate getMinPixel() const = 0;
  virtual PixelCoordinate getMaxPixel() const = 0;
};

class EllipticalAperture : public EAperture {
public:
  virtual ~EllipticalAperture() = default;
  EllipticalAperture(SeFloat center_x, SeFloat center_y, SeFloat cxx, SeFloat cyy, SeFloat cxy, SeFloat rad_max) :
    m_center_x(center_x),
    m_center_y(center_y),
    m_cxx(cxx),
    m_cyy(cyy),
    m_cxy(cxy),
    m_rad_max(rad_max) {}

  virtual SeFloat getAreaSub(int pixel_x, int pixel_y) const override;
  virtual int getArea(int pixel_x, int pixel_y) const override;
  virtual SeFloat getRadiusSquared(int pixel_x, int pixel_y) const;
  virtual PixelCoordinate getMinPixel() const override;
  virtual PixelCoordinate getMaxPixel() const override;

private:
  SeFloat m_center_x;
  SeFloat m_center_y;
  SeFloat m_cxx;
  SeFloat m_cyy;
  SeFloat m_cxy;
  SeFloat m_rad_max;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYTASK_H_ */

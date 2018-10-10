/*
 * AperturePhotometryTask.h
 *
 *  Created on: Sep 22, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYTASK_H_

#include "SEFramework/Aperture/Aperture.h"
#include "SEFramework/Task/SourceTask.h"

namespace SExtractor {

class AperturePhotometryTask : public SourceTask {
public:

  virtual ~AperturePhotometryTask() = default;

  AperturePhotometryTask(std::shared_ptr<Aperture> aperture, unsigned int instance, unsigned int image_instance,
                         SeFloat magnitude_zero_point, bool use_symmetry)
    : m_aperture(aperture),
      m_instance(instance),
      m_image_instance(image_instance),
      m_magnitude_zero_point(magnitude_zero_point),
      m_use_symmetry(use_symmetry) {}

  virtual void computeProperties(SourceInterface &source) const override;

private:
  std::shared_ptr<Aperture> m_aperture;
  unsigned int m_instance;
  unsigned int m_image_instance;
  SeFloat m_magnitude_zero_point;
  bool m_use_symmetry;

};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYTASK_H_ */

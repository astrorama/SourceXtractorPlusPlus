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

  AperturePhotometryTask(const std::vector<SeFloat> &apertures, unsigned int instance,
                         SeFloat magnitude_zero_point, bool use_symmetry)
    : m_apertures(apertures),
      m_instance(instance),
      m_magnitude_zero_point(magnitude_zero_point),
      m_use_symmetry(use_symmetry) {}

  virtual void computeProperties(SourceInterface &source) const override;

private:
  std::vector<SeFloat> m_apertures;
  unsigned int m_instance;
  SeFloat m_magnitude_zero_point;
  bool m_use_symmetry;

};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYTASK_H_ */

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
 * AperturePhotometryTask.h
 *
 *  Created on: Sep 22, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYTASK_H_

#include "SEFramework/Aperture/Aperture.h"
#include "SEFramework/Task/SourceTask.h"

namespace SourceXtractor {

class AperturePhotometryTask : public SourceTask {
public:

  virtual ~AperturePhotometryTask() = default;

  AperturePhotometryTask(const std::vector<SeFloat> &apertures, unsigned int instance,
                         SeFloat magnitude_zero_point, bool use_symmetry)
    : m_apertures(apertures),
      m_instance(instance),
      m_magnitude_zero_point(magnitude_zero_point),
      m_use_symmetry(use_symmetry) {}

  void computeProperties(SourceInterface &source) const override;

private:
  std::vector<SeFloat> m_apertures;
  unsigned int m_instance;
  SeFloat m_magnitude_zero_point;
  bool m_use_symmetry;

};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYTASK_H_ */

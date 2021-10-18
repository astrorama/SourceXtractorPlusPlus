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
 * ApertureFlagTask.h
 *
 *  Created on: Oct 09, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREFLAGTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREFLAGTASK_H_

#include "SEFramework/Aperture/Aperture.h"
#include "SEFramework/Task/SourceTask.h"

namespace SourceXtractor {

class ApertureFlagTask : public SourceTask {
public:

  virtual ~ApertureFlagTask() = default;

  explicit ApertureFlagTask(const std::vector<SeFloat> &apertures)
    : m_apertures(apertures) {}

  virtual void computeProperties(SourceInterface &source) const override;

private:
  std::vector<SeFloat> m_apertures;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREFLAGTASK_H_ */

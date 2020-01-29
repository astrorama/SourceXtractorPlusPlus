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

/**
 * @file SNRRatioSourceTask.h
 *
 * @date Apr 29, 2020
 * @author mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_SNRRATIOSOURCETASK_H_
#define _SEIMPLEMENTATION_PLUGIN_SNRRATIOSOURCETASK_H_

#include "SEFramework/Task/SourceTask.h"
#include "SEImplementation/Plugin/SNRRatio/SNRRatio.h"
#include "SEImplementation/Plugin/IsophotalFlux/IsophotalFlux.h"

namespace SourceXtractor {
class SNRRatioSourceTask : public SourceTask {
public:
  virtual ~SNRRatioSourceTask() = default;

  virtual void computeProperties(SourceInterface& source) const {
    // get the input quantities
    const auto& iso_flux = source.getProperty<IsophotalFlux>().getFlux();
    const auto& iso_flux_error = source.getProperty<IsophotalFlux>().getFluxError();

    // compute and store the property
    SeFloat snr_ratio = iso_flux / iso_flux_error;
    source.setProperty<SNRRatio>(snr_ratio);
  };
private:
}; // End of SNRRatioSourceTask class
} // namespace SourceXtractor

#endif /* _SEIMPLEMENTATION_PLUGIN_SNRRATIOSOURCETASK_H_ */



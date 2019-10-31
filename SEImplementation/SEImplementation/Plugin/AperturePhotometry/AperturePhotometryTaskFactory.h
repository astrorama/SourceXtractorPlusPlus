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
 * AperturePhotometryTaskFactory.h
 *
 *  Created on: Sep 23, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYTASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYTASKFACTORY_H_

#include "SEFramework/Task/TaskFactory.h"
#include "SEUtils/Types.h"

namespace SourceXtractor {

/**
 * The configuration of this plugin can be kind of convoluted, so some clarifications may be needed:
 * We have a set of frames, and each frame has a set of apertures. These set of
 * apertures are aggregated under one single column.
 * So we need to compute the set of apertures per image per source (AperturePhotometry),
 * but we need to gather and group then into multidimensional columns (AperturePhotometryArray).
 * Since there can be more than one column, there can be more than one instance of AperturePhotometryArray,
 * each one corresponding to a column.
 * For instance, we may have 4 frames, so there will be 4 instances of AperturePhotometry.
 * Since they belong to two bands, there will be two columns aperture_r and aperture_g.
 * aperture_r (instance 0) needs to "gather" the apertures for frames 0 and 1, and aperture_g (instance 1) from
 * 2 and 3.
 * Thus, handling AperturePhotometry is easy, but for AperturePhotometryArray (the output), we need to register
 * the converter with two different instances, keeping a map between the instance of AperturePhotometryArray and
 * the instances of AperturePhotometry that are aggregated.
 */
class AperturePhotometryTaskFactory : public TaskFactory {
public:
  AperturePhotometryTaskFactory() : m_magnitude_zero_point(0), m_symmetry_usage(true) {}

  /// Destructor
  virtual ~AperturePhotometryTaskFactory() = default;

  void reportConfigDependencies(Euclid::Configuration::ConfigManager &manager) const override;

  void configure(Euclid::Configuration::ConfigManager &manager) override;

  virtual std::shared_ptr<Task> createTask(const PropertyId &property_id) const override;

  void registerPropertyInstances(OutputRegistry &) override;

private:
  SeFloat m_magnitude_zero_point;
  bool m_symmetry_usage;

  // Apertures for a given image ID
  std::map<unsigned, std::vector<float>> m_aperture_config;
  // All configured apertures (required to be able to flag on the detection image)
  std::vector<float> m_all_apertures;

  std::map<unsigned, std::string> m_col_prefix;
  std::map<unsigned, std::vector<unsigned>> m_apertures_per_output;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_APERTUREPHOTOMETRY_APERTUREPHOTOMETRYTASKFACTORY_H_ */

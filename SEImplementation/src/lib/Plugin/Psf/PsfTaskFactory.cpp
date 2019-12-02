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
 * PsfTaskFactory.h
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro Álvarez Ayllón
 */

#include "SEImplementation/Configuration/MeasurementImageConfig.h"
#include "SEImplementation/Plugin/Psf/PsfTaskFactory.h"

namespace SourceXtractor {

void PsfTaskFactory::reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const {
  manager.registerConfiguration<PsfPluginConfig>();
  manager.registerConfiguration<MeasurementImageConfig>();
}

void PsfTaskFactory::configure(Euclid::Configuration::ConfigManager& manager) {
  auto psf_config = manager.getConfiguration<PsfPluginConfig>();
  auto measurement_config = manager.getConfiguration<MeasurementImageConfig>();

  const auto& default_psf = psf_config.getPsf();
  const auto& image_infos = manager.getConfiguration<MeasurementImageConfig>().getImageInfos();

  for (unsigned int i = 0; i < image_infos.size(); i++) {
    if (!image_infos[i].m_psf_path.empty()) {
      m_vpsf[image_infos[i].m_id] = PsfPluginConfig::readPsf(image_infos[i].m_psf_path, image_infos[i].m_psf_hdu);
    }
    else if (default_psf) {
      m_vpsf[image_infos[i].m_id] = default_psf;
    }
  }

  for (auto& vpsf : m_vpsf) {
    if (!vpsf.second) {
      throw Elements::Exception() << "Missing PSF. Make sure every frame has a PSF, or that there is a valid default PSF";
    }
  }
}

std::shared_ptr<Task> PsfTaskFactory::createTask(const SourceXtractor::PropertyId& property_id) const {
  auto instance = property_id.getIndex();

  if (m_vpsf.find(instance) == m_vpsf.end() || !m_vpsf.at(instance)) {
    throw Elements::Exception() << "Missing PSF. Make sure every frame has a PSF";
  }

  try {
    return std::make_shared<PsfTask>(instance, m_vpsf.at(instance));
  } catch (const std::out_of_range&) {
    return nullptr;
  }
}

} // end SourceXtractor

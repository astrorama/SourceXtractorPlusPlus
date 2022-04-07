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
 * PsfTask.cpp
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include <numeric>
#include "SEImplementation/Plugin/Psf/PsfProperty.h"
#include "SEImplementation/Plugin/MeasurementFrameGroupRectangle/MeasurementFrameGroupRectangle.h"
#include "SEImplementation/CheckImages/CheckImages.h"
#include "SEImplementation/Image/WriteableImageInterfaceTraits.h"
#include "SEImplementation/Plugin/Psf/PsfTask.h"

namespace SourceXtractor {

static double getCoordX(SourceXtractor::SourceGroupInterface& group, unsigned instance) {
  auto& measurement_frame_group = group.getProperty<MeasurementFrameGroupRectangle>(instance);
  double top_x = measurement_frame_group.getTopLeft().m_x;
  return top_x + measurement_frame_group.getWidth() / 2.;
}

static double getCoordY(SourceXtractor::SourceGroupInterface& group, unsigned instance) {
  auto& measurement_frame_group = group.getProperty<MeasurementFrameGroupRectangle>(instance);
  double top_y = measurement_frame_group.getTopLeft().m_y;
  return top_y + measurement_frame_group.getHeight() / 2.;
}

std::map<std::string, PsfTask::ValueGetter> PsfTask::component_value_getters{
  {"X_IMAGE",      getCoordX},
  {"Y_IMAGE",      getCoordY},
  {"XWIN_IMAGE",   getCoordX},
  {"YWIN_IMAGE",   getCoordY},
  {"XPEAK_IMAGE",  getCoordX},
  {"YPEAK_IMAGE",  getCoordY},
  {"XMODEL_IMAGE", getCoordX},
  {"YMODEL_IMAGE", getCoordY}
};

PsfTask::PsfTask(unsigned instance, const std::shared_ptr<Psf> &vpsf)
    : m_instance(instance), m_vpsf(vpsf) {
}

void PsfTask::computeProperties(SourceXtractor::SourceGroupInterface &group) const {
  if (m_vpsf != nullptr) {
    std::vector<double> component_values;

    for (auto component : m_vpsf->getComponents()) {
      component_values.push_back(component_value_getters[component](group, m_instance));
    }

    auto psf = m_vpsf->getPsf(component_values);
    // The result may not be normalized!
    auto psf_sum = std::accumulate(psf->getData().begin(), psf->getData().end(), 0.);
    auto psf_normalized = VectorImage<SeFloat>::create(*MultiplyImage<SeFloat>::create(psf, 1. / psf_sum));
    group.setIndexedProperty<PsfProperty>(m_instance, m_vpsf->getPixelSampling(), psf_normalized);

    // Check image
    if (group.size()) {
      auto check_image = CheckImages::getInstance().getPsfImage(m_instance);
      if (check_image) {
        auto x = component_value_getters["X_IMAGE"](group, m_instance);
        auto y = component_value_getters["Y_IMAGE"](group, m_instance);

        ModelFitting::ImageTraits<ModelFitting::WriteableInterfaceTypePtr>::addImageToImage(
          check_image, psf_normalized, m_vpsf->getPixelSampling(), x, y);
      }
    }
  } else {
    group.setIndexedProperty<PsfProperty>(m_instance, 1.0, nullptr);
  }
}

}

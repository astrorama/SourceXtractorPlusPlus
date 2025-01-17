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

#include <numeric>
#include "SEImplementation/Plugin/SourcePsf/SourcePsfProperty.h"
#include "SEImplementation/Plugin/MeasurementFramePixelCentroid/MeasurementFramePixelCentroid.h"
#include "SEImplementation/CheckImages/CheckImages.h"
#include "SEImplementation/Image/WriteableImageInterfaceTraits.h"
#include "SEImplementation/Plugin/SourcePsf/SourcePsfTask.h"

namespace SourceXtractor {

static double getCoordX(SourceXtractor::SourceInterface& source, unsigned instance) {
  auto& pixel_centroid = source.getProperty<MeasurementFramePixelCentroid>(instance);
  return pixel_centroid.getCentroidX();
}

static double getCoordY(SourceXtractor::SourceInterface& source, unsigned instance) {
  auto& pixel_centroid = source.getProperty<MeasurementFramePixelCentroid>(instance);
  return pixel_centroid.getCentroidY();
}

std::map<std::string, SourcePsfTask::ValueGetter> SourcePsfTask::component_value_getters{
  {"X_IMAGE",      getCoordX},
  {"Y_IMAGE",      getCoordY},
  {"XWIN_IMAGE",   getCoordX},
  {"YWIN_IMAGE",   getCoordY},
  {"XPEAK_IMAGE",  getCoordX},
  {"YPEAK_IMAGE",  getCoordY},
  {"XMODEL_IMAGE", getCoordX},
  {"YMODEL_IMAGE", getCoordY}
};

SourcePsfTask::SourcePsfTask(unsigned instance, const std::shared_ptr<Psf> &vpsf)
    : m_instance(instance), m_vpsf(vpsf) {
}

void SourcePsfTask::computeProperties(SourceXtractor::SourceInterface &source) const {
  if (m_vpsf != nullptr) {
    std::vector<double> component_values;

    for (auto component : m_vpsf->getComponents()) {
      component_values.push_back(component_value_getters[component](source, m_instance));
    }

    auto psf = m_vpsf->getPsf(component_values);
    std::shared_ptr<VectorImage<SeFloat>> psf_normalized = psf;

    if (m_normalize_psf) {
      // The result may not be normalized!
      auto psf_sum = std::accumulate(psf->getData().begin(), psf->getData().end(), 0.);
      psf_normalized = VectorImage<SeFloat>::create(*MultiplyImage<SeFloat>::create(psf, 1. / psf_sum));
    }
    source.setIndexedProperty<SourcePsfProperty>(m_instance, m_vpsf->getPixelSampling(), psf_normalized);

    // Check image
    auto check_image = CheckImages::getInstance().getPsfImage(m_instance);
    if (check_image) {
      auto x = component_value_getters["X_IMAGE"](source, m_instance);
      auto y = component_value_getters["Y_IMAGE"](source, m_instance);

      ModelFitting::ImageTraits<ModelFitting::WriteableInterfaceTypePtr>::addImageToImage(
        check_image, psf_normalized, m_vpsf->getPixelSampling(), x, y);
    }
  } else {
    source.setIndexedProperty<SourcePsfProperty>(m_instance, 1.0, nullptr);
  }
}

}

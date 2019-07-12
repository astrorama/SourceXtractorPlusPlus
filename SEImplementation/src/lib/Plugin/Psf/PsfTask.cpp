/*
 * PsfTask.cpp
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include <numeric>
#include "SEImplementation/Measurement/MultithreadedMeasurement.h"
#include "SEImplementation/Plugin/Psf/PsfProperty.h"
#include "SEImplementation/Plugin/MeasurementFrameGroupRectangle/MeasurementFrameGroupRectangle.h"
#include "SEImplementation/CheckImages/CheckImages.h"
#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Image/WriteableImageInterfaceTraits.h"
#include "SEImplementation/Plugin/Psf/PsfTask.h"

namespace SExtractor {


std::map<std::string, ValueGetter> component_value_getters {
    {"X_IMAGE", [](SExtractor::SourceGroupInterface &group, unsigned instance){
      auto& measurement_frame_group = group.getProperty<MeasurementFrameGroupRectangle>(instance);
      double top_x = measurement_frame_group.getTopLeft().m_x;
      return top_x + measurement_frame_group.getWidth() / 2.;
    }},
    {"Y_IMAGE", [](SExtractor::SourceGroupInterface &group, unsigned instance){
      auto& measurement_frame_group = group.getProperty<MeasurementFrameGroupRectangle>(instance);
      double top_y = measurement_frame_group.getTopLeft().m_y;
      return top_y + measurement_frame_group.getHeight() / 2.;
    }}
};

PsfTask::PsfTask(unsigned instance, const std::shared_ptr<VariablePsf> &vpsf)
    : m_instance(instance), m_vpsf(vpsf) {
}

void PsfTask::computeProperties(SExtractor::SourceGroupInterface &group) const {
  std::vector<double> component_values;

  for (auto c : m_vpsf->getComponents()) {
    component_values.push_back(component_value_getters[c.name](group, m_instance));
  }

  auto psf = m_vpsf->getPsf(component_values);
  // The result may not be normalized!
  auto psf_sum = std::accumulate(psf->getData().begin(), psf->getData().end(), 0.);
  auto psf_normalized = VectorImage<SeFloat>::create(*MultiplyImage<SeFloat>::create(psf, 1. / psf_sum));
  group.setIndexedProperty<PsfProperty>(m_instance, ImagePsf{m_vpsf->getPixelScale(), psf_normalized});

  // Check image
  if (group.size()) {
    auto& frame = group.begin()->getProperty<MeasurementFrame>(m_instance);
    auto check_image = CheckImages::getInstance().getPsfImage(frame.getFrame());
    if (check_image) {
      std::lock_guard<std::recursive_mutex> lock(MultithreadedMeasurement::g_global_mutex);

      auto x = component_value_getters["X_IMAGE"](group, m_instance);
      auto y = component_value_getters["Y_IMAGE"](group, m_instance);

      ModelFitting::ImageTraits<ModelFitting::WriteableInterfaceTypePtr>::addImageToImage(
        check_image, psf_normalized, m_vpsf->getPixelScale(), x, y);
    }
  }
}

}

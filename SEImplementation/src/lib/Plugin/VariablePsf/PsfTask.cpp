/*
 * PsfTask.cpp
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#include <SEImplementation/Plugin/Psf/PsfProperty.h>
#include <SEImplementation/Plugin/MeasurementFrameGroupRectangle/MeasurementFrameGroupRectangle.h>
#include <numeric>
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
}

}

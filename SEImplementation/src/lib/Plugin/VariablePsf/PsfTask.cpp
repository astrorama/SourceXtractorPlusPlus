/*
 * PsfTask.cpp
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro Álvarez Ayllón
 */

#include <SEImplementation/Plugin/Psf/PsfProperty.h>
#include <SEImplementation/Plugin/DetectionFrameGroupStamp/DetectionFrameGroupStamp.h>
#include <numeric>
#include "SEImplementation/Plugin/Psf/PsfTask.h"

namespace SExtractor {


std::map<std::string, ValueGetter> component_value_getters {
    {"X_IMAGE", [](SExtractor::SourceGroupInterface &group){
      auto &detection_frame_group = group.getProperty<DetectionFrameGroupStamp>();
      auto top_x = detection_frame_group.getTopLeft().m_x;
      return top_x + detection_frame_group.getStamp().getWidth() / 2;
    }},
    {"Y_IMAGE", [](SExtractor::SourceGroupInterface &group){
      auto &detection_frame_group = group.getProperty<DetectionFrameGroupStamp>();
      auto top_y = detection_frame_group.getTopLeft().m_y;
      return top_y + detection_frame_group.getStamp().getHeight() / 2;
    }}
};

PsfTask::PsfTask(unsigned instance, const std::shared_ptr<VariablePsf> &vpsf)
    : m_instance(instance), m_vpsf(vpsf) {
}

void PsfTask::computeProperties(SExtractor::SourceGroupInterface &group) const {
  std::vector<double> component_values;

  for (auto c : m_vpsf->getComponents()) {
    component_values.push_back(component_value_getters[c.name](group));
  }

  auto psf = m_vpsf->getPsf(component_values);
  // The result may not be normalized!
  auto psf_sum = std::accumulate(psf->getData().begin(), psf->getData().end(), 0.);
  auto psf_div = ConstantImage<SeFloat>::create(psf->getWidth(), psf->getHeight(), 1. / psf_sum);
  auto psf_normalized = VectorImage<SeFloat>::create(*MultiplyImage<SeFloat>::create(psf, psf_div));
  group.setIndexedProperty<PsfProperty>(m_instance, ImagePsf{m_vpsf->getPixelScale(), psf_normalized});
}

}

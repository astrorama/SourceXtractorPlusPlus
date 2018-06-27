/*
 * PsfTask.cpp
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro Álvarez Ayllón
 */

#include <SEImplementation/Plugin/PixelCentroid/PixelCentroid.h>
#include <SEImplementation/Plugin/Psf/PsfProperty.h>
#include "SEImplementation/Plugin/Psf/PsfTask.h"

namespace SExtractor {


std::map<std::string, ValueGetter> component_value_getters {
    {"X_IMAGE", [](SExtractor::SourceInterface &source){
      return source.getProperty<PixelCentroid>().getCentroidX();
    }},
    {"Y_IMAGE", [](SExtractor::SourceInterface &source){
      return source.getProperty<PixelCentroid>().getCentroidY();
    }}
};

PsfTask::PsfTask(const std::shared_ptr<VariablePsf> &vpsf): m_vpsf(vpsf) {
}

void PsfTask::computeProperties(SExtractor::SourceInterface &source) const {
  std::vector<double> component_values;

  for (auto c : m_vpsf->getComponents()) {
    component_values.push_back(component_value_getters[c.name](source));
  }

  auto psf = m_vpsf->getPsf(component_values);
  source.setProperty<PsfProperty>(ImagePsf{m_vpsf->getPixelScale(), psf});
}

}

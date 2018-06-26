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

PsfTask::PsfTask(const std::shared_ptr<VariablePsf> &vpsf): m_vpsf(vpsf) {
}

void PsfTask::computeProperties(SExtractor::SourceInterface &source) const {
  auto centroid = source.getProperty<PixelCentroid>();
  auto psf = m_vpsf->getPsf({centroid.getCentroidX(), centroid.getCentroidY()});
  source.setProperty<PsfProperty>(ImagePsf{m_vpsf->getPixelScale(), psf});
}

}

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

#include "SEImplementation/Plugin/MeasurementFrame/MeasurementFrame.h"
#include "SEImplementation/Plugin/MeasurementFrameImages/MeasurementFrameImages.h"
#include "SEImplementation/Plugin/MeasurementFrameImages/MeasurementFrameImagesTask.h"

namespace SourceXtractor {

void MeasurementFrameImagesTask::computeProperties(SourceInterface& source) const {
  auto frame = source.getProperty<MeasurementFrame>(m_instance).getFrame();
  auto width = frame->getOriginalImage()->getWidth();
  auto height = frame->getOriginalImage()->getHeight();

  source.setIndexedProperty<MeasurementFrameImages>(m_instance, frame, width, height);
}

} // SEImplementation namespace


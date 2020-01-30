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
/**
 * @file src/lib/Pipeline/Segmentation.cpp
 * @date 09/07/16
 * @author mschefer
 */

#include "SEFramework/Pipeline/Segmentation.h"

namespace SourceXtractor {

Segmentation::Segmentation(std::shared_ptr<DetectionImageFrame::ImageFilter> image_processing)
    : m_filter_image_processing(image_processing) {
}

void Segmentation::processFrame(std::shared_ptr<DetectionImageFrame> frame) const {
  if (m_filter_image_processing != nullptr) {
    frame->setFilter(m_filter_image_processing);
  }

  if (m_labelling != nullptr) {
    LabellingListener listener(*this, frame);
    m_labelling->labelImage(listener, frame);
  }

  // Flush source grouping buffer
  SelectAllCriteria select_all_criteria;
  Observable<ProcessSourcesEvent>::notifyObservers(ProcessSourcesEvent(select_all_criteria));
}

}

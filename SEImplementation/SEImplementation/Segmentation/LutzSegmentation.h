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
 * @file Segmentation/Lutz/Lutz.h
 * @date 05/31/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_SEGMENTATION_LUTZ_SEGMENTATION_H
#define _SEIMPLEMENTATION_SEGMENTATION_LUTZ_SEGMENTATION_H

#include <cassert>
#include <memory>
#include "SEFramework/Frame/Frame.h"
#include "SEFramework/Source/SourceFactory.h"
#include "SEFramework/Pipeline/Segmentation.h"

namespace SourceXtractor {

/**
 * @class Lutz
 * @brief Implements a Segmentation based on the Lutz algorithm
 */
class LutzSegmentation : public Segmentation::Labelling {

public:

  /**
   * @brief Destructor
   */
  virtual ~LutzSegmentation() = default;

  LutzSegmentation(std::shared_ptr<SourceFactory> source_factory, int window_size = 0)
      : m_source_factory(source_factory),
        m_window_size(window_size) {
    assert(source_factory != nullptr);
  }

  virtual void labelImage(Segmentation::LabellingListener& listener, std::shared_ptr<const DetectionImageFrame> frame) override;

private:
  std::shared_ptr<SourceFactory> m_source_factory;
  int m_window_size;
};

} /* namespace SourceXtractor */

#endif

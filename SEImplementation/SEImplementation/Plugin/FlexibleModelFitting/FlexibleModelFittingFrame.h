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
 * FlexibleModelFittingFrame.h
 *
 *  Created on: Sep 17, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGFRAME_H_
#define _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGFRAME_H_

#include <memory>
#include <vector>

#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingModel.h"

namespace SourceXtractor {

class FlexibleModelFittingFrame {
public:

  FlexibleModelFittingFrame(unsigned int frame_nb,
      const std::vector<std::shared_ptr<FlexibleModelFittingModel>>& models)
          : m_frame_nb(frame_nb), m_models(models) {}

  virtual ~FlexibleModelFittingFrame() {}

  unsigned int getFrameNb() const {
    return m_frame_nb;
  }

  std::vector<std::shared_ptr<FlexibleModelFittingModel>> getModels() const {
    return m_models;
  }

private:
  unsigned int m_frame_nb;
  std::vector<std::shared_ptr<FlexibleModelFittingModel>> m_models;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGFRAME_H_ */

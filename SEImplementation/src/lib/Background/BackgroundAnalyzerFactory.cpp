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
 * BackgroundAnalyzerFactory.cpp
 *
 *  Created on: Mar 29, 2017
 *      Author: mschefer
 */


#include "SEImplementation/Background/BackgroundAnalyzerFactory.h"

#include "SEImplementation/Background/SimpleBackgroundAnalyzer.h"
#include "SEImplementation/Background/SE/SEBackgroundLevelAnalyzer.h"
#include "SEImplementation/Background/SE2/SE2BackgroundLevelAnalyzer.h"

namespace SourceXtractor {

std::shared_ptr<BackgroundAnalyzer> BackgroundAnalyzerFactory::createBackgroundAnalyzer() const {
  return createBackgroundAnalyzer(m_weight_type);
}

std::shared_ptr<BackgroundAnalyzer> BackgroundAnalyzerFactory::createBackgroundAnalyzer(
    WeightImageConfig::WeightType weight_type) const {
  // make a SE2 background if cell size and smoothing box are given
  if (m_cell_size.size() > 0 && m_smoothing_box.size() > 0) {
    if (m_legacy)
      return std::make_shared<SE2BackgroundLevelAnalyzer>(m_cell_size, m_smoothing_box, weight_type);
    else
      return std::make_shared<SEBackgroundLevelAnalyzer>(m_cell_size, m_smoothing_box, weight_type);
  } else {
    // make a simple background
    return std::make_shared<SimpleBackgroundAnalyzer>();
  }
}

BackgroundAnalyzerFactory::BackgroundAnalyzerFactory(long manager_id) : Configuration(manager_id) {
  declareDependency<SE2BackgroundConfig>();
  declareDependency<WeightImageConfig>();
}

void BackgroundAnalyzerFactory::initialize(const UserValues&) {
  auto se2background_config = getDependency<SE2BackgroundConfig>();
  auto weight_image_config = getDependency<WeightImageConfig>();
  m_cell_size = se2background_config.getCellSize();
  m_smoothing_box = se2background_config.getSmoothingBox();
  m_legacy = se2background_config.useLegacy();
  m_weight_type = weight_image_config.getWeightType();
}

}

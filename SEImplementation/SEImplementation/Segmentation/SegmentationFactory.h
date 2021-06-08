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
 * @file SEImplementation/SegmentationFactory.h
 * @date 06/08/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_SEGMENTATIONFACTORY_H
#define _SEIMPLEMENTATION_SEGMENTATIONFACTORY_H

#include "SEImplementation/Configuration/SegmentationConfig.h"

#include "SEFramework/Task/TaskProvider.h"
#include "SEFramework/Configuration/Configurable.h"
#include "SEFramework/Pipeline/Segmentation.h"

namespace SourceXtractor {

/**
 * @class SegmentationFactory
 * @brief The SegmentationFactory will provide a Segmentation implementation based on the current configuration
 *
 */
class SegmentationFactory : public Configurable {

public:

  /// Destructor
  virtual ~SegmentationFactory() = default;

  /// Constructor
  SegmentationFactory(std::shared_ptr<TaskProvider> task_provider);

  std::shared_ptr<Segmentation> createSegmentation() const;
  
  // Implementation of the Configurable interface
  void configure(Euclid::Configuration::ConfigManager& manager) override;
  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;

private:
  std::shared_ptr<DetectionImageFrame::ImageFilter> m_filter;

  SegmentationConfig::Algorithm m_algorithm;
  std::shared_ptr<TaskProvider> m_task_provider;

  int m_lutz_window_size;
  int m_bfs_max_delta;

  std::string m_model_path;
  double m_ml_threshold;

}; /* End of SegmentationFactory class */

} /* namespace SourceXtractor */


#endif

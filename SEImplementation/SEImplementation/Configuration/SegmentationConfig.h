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
 * @file SEImplementation/Configuration/SegmentationConfig.h
 * @date 06/08/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_SEGMENTATIONCONFIG_H
#define _SEIMPLEMENTATION_SEGMENTATIONCONFIG_H

#include <memory>
#include "Configuration/Configuration.h"

namespace SourceXtractor {

/**
 * @class SegmentationConfig
 * @brief Used to select a Segmentation algorithm.
 *
 */
class SegmentationConfig : public Euclid::Configuration::Configuration {
public:

  enum class Algorithm {
    UNKNOWN,
    LUTZ
  };

  /**
   * @brief Destructor
   */
  virtual ~SegmentationConfig() = default;

  /// Constructs a new SegmentationConfig object
  SegmentationConfig(long manager_id);

  std::map<std::string, Configuration::OptionDescriptionList> getProgramOptions() override;
  void preInitialize(const UserValues& args) override;
  void initialize(const UserValues& args) override;

  Algorithm getAlgorithmOption() const {
    return m_selected_algorithm;
  }

  int getLutzWindowSize() const {
    return m_lutz_window_size;
  }

  bool isFilteringEnabled() const {
    return m_filter != nullptr;
  }

  std::shared_ptr<DetectionImageFrame::ImageFilter> getFilter() const {
    return m_filter;
  }

private:
  std::shared_ptr<DetectionImageFrame::ImageFilter> getDefaultFilter() const;
  std::shared_ptr<DetectionImageFrame::ImageFilter> loadFilter(const std::string& filename) const;
  std::shared_ptr<DetectionImageFrame::ImageFilter> loadFITSFilter(const std::string& filename) const;
  std::shared_ptr<DetectionImageFrame::ImageFilter> loadASCIIFilter(const std::string& filename) const;

  Algorithm m_selected_algorithm;
  std::shared_ptr<DetectionImageFrame::ImageFilter> m_filter;

  int m_lutz_window_size;
}; /* End of SegmentationConfig class */

} /* namespace SourceXtractor */


#endif

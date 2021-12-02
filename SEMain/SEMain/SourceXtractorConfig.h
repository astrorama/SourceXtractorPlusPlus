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
 * @file SEMain/SourceXtractorConfig.h
 * @date 07/20/16
 * @author nikoapos
 */

#ifndef _SEMAIN_SOURCEXTRACTORCONFIG_H
#define _SEMAIN_SOURCEXTRACTORCONFIG_H

#include "Configuration/Configuration.h"

namespace SourceXtractor {

/**
 * @class SourceXtractorConfig
 * @brief
 *
 */
class SourceXtractorConfig : public Euclid::Configuration::Configuration {

public:

  /**
   * @brief Destructor
   */
  virtual ~SourceXtractorConfig() = default;

  explicit SourceXtractorConfig(long manager_id);

}; /* End of SourceXtractorConfig class */

} /* namespace SourceXtractor */


#endif

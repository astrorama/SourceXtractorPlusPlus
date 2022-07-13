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
 * SourceFactory.h
 *
 *  Created on: Aug 8, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_SOURCE_SOURCEFACTORY_H_
#define _SEFRAMEWORK_SOURCE_SOURCEFACTORY_H_

#include <memory>

namespace SourceXtractor {

class SourceInterface;

/**
 * @class SourceFactory
 * @brief A factory interface to create SourceInterface instances
 *
 */

class SourceFactory {
public:
  virtual ~SourceFactory() = default;

  virtual std::unique_ptr<SourceInterface> createSource() const = 0;
};

}


#endif /* _SEFRAMEWORK_SOURCE_SOURCEFACTORY_H_ */

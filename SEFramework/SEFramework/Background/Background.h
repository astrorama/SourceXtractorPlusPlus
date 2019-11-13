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
 * Background.h
 *
 *  Created on: Mar 28, 2017
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_BACKGROUND_BACKGROUND_H_
#define _SEFRAMEWORK_BACKGROUND_BACKGROUND_H_

#include "SEFramework/Image/Image.h"

namespace SExtractor {

class Background {
public:

  virtual ~Background() = default;

  virtual std::shared_ptr<Image<SeFloat>> getBackgroundLevelImage() const = 0;
  virtual std::shared_ptr<Image<SeFloat>> getAutoWeightImage() const = 0;


  virtual SeFloat getBackroundRMS() const = 0;
  virtual SeFloat getBackroundDetectionThreshold() const = 0;
};


}



#endif /* _SEFRAMEWORK_BACKGROUND_BACKGROUND_H_ */

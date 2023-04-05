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
 * WorldCentroid.h
 *
 *  Created on: Nov 21, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_WORLDCENTROID_WORLDCENTROID_H_
#define _SEIMPLEMENTATION_PLUGIN_WORLDCENTROID_WORLDCENTROID_H_

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"

namespace SourceXtractor {

class WorldCentroid : public Property {
public:

  virtual ~WorldCentroid() = default;

  WorldCentroid(SeDouble centroid_alpha, SeDouble centroid_delta)
      : m_centroid_alpha(centroid_alpha), m_centroid_delta(centroid_delta) {}

  SeDouble getCentroidAlpha() const {
    return m_centroid_alpha;
  }

  SeDouble getCentroidDelta() const {
    return m_centroid_delta;
  }

private:
  SeDouble m_centroid_alpha, m_centroid_delta;

};

}

#endif

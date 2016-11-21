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

namespace SExtractor {

class WorldCentroid : public Property {
public:

  virtual ~WorldCentroid() = default;

  WorldCentroid(SeFloat centroid_alpha, SeFloat centroid_delta)
      : m_centroid_alpha(centroid_alpha), m_centroid_delta(centroid_delta) {}

  SeFloat getCentroidAlpha() const {
    return m_centroid_alpha;
  }

  SeFloat getCentroidDelta() const {
    return m_centroid_delta;
  }

private:
  SeFloat m_centroid_alpha, m_centroid_delta;

};

}

#endif

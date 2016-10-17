/*
 * Background.h
 *
 *  Created on: Oct 11, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_BACKGROUND_BACKGROUND_H_
#define _SEIMPLEMENTATION_BACKGROUND_BACKGROUND_H_

#include "SEFramework/Image/Image.h"

namespace SExtractor {

class Background {
public:

  virtual ~Background() = default;

  Background(std::shared_ptr<DetectionImage> detection_image) : m_detection_image(detection_image) {}

  SeFloat getMedian() const;
  SeFloat getRMS(SeFloat background_value) const;

private:
  std::shared_ptr<DetectionImage> m_detection_image;
};

}

#endif /* _SEIMPLEMENTATION_BACKGROUND_BACKGROUND_H_ */

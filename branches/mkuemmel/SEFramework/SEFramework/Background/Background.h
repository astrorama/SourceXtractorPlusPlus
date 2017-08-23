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

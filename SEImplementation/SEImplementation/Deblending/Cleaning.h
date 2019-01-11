/*
 * Cleaning.h
 *
 *  Created on: 2018 M12 18
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_DEBLENDING_CLEANING_H_
#define _SEIMPLEMENTATION_DEBLENDING_CLEANING_H_

#include "SEFramework/Pipeline/Deblending.h"

namespace SExtractor {

class Cleaning : public DeblendStep {

public:
  Cleaning() {}
  virtual ~Cleaning() {}

  void deblend(SourceGroupInterface& group) const override;

private:

};

}


#endif /* _SEIMPLEMENTATION_DEBLENDING_CLEANING_H_ */

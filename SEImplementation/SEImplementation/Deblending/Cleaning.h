/*
 * Cleaning.h
 *
 *  Created on: 2018 M12 18
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_DEBLENDING_CLEANING_H_
#define _SEIMPLEMENTATION_DEBLENDING_CLEANING_H_

#include "SEFramework/Pipeline/Deblending.h"
#include "SEFramework/Source/SourceInterface.h"
#include "SEFramework/Source/SourceGroupInterface.h"

namespace SExtractor {

class Cleaning : public DeblendStep {

public:
  Cleaning() {}
  virtual ~Cleaning() {}

  void deblend(SourceGroupInterface& group) const override;

private:
    bool shouldClean(SourceInterface& source, SourceGroupInterface& group) const;

};

}


#endif /* _SEIMPLEMENTATION_DEBLENDING_CLEANING_H_ */

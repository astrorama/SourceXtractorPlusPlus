/*
 * Cleaning.h
 *
 *  Created on: 2018 M12 18
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_DEBLENDING_CLEANING_H_
#define _SEIMPLEMENTATION_DEBLENDING_CLEANING_H_

#include "SEUtils/Types.h"
#include "SEFramework/Pipeline/Deblending.h"
#include "SEFramework/Source/SourceInterface.h"
#include "SEFramework/Source/SourceGroupInterface.h"

namespace ModelFitting {
  class ExtendedModel;
}

namespace SExtractor {

class MoffatModelFitting;

class Cleaning : public DeblendStep {

public:
  Cleaning() {}
  virtual ~Cleaning() {}

  void deblend(SourceGroupInterface& group) const override;

private:
    bool shouldClean(SourceInterface& source, SourceGroupInterface& group) const;
    ModelFitting::ExtendedModel createMoffatModel(SourceInterface& source) const;
};

}


#endif /* _SEIMPLEMENTATION_DEBLENDING_CLEANING_H_ */

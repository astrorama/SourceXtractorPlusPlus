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
#include "SEFramework/Source/SourceFactory.h"

namespace ModelFitting {
  class ExtendedModel;
}

namespace SExtractor {

class MoffatModelFitting;

class Cleaning : public DeblendStep {

public:
  Cleaning(std::shared_ptr<SourceFactory> source_factory, unsigned int min_area) :
    m_source_factory(source_factory), m_min_area(min_area) {}
  virtual ~Cleaning() {}

  void deblend(SourceGroupInterface& group) const override;

private:
  SourceGroupInterface::iterator findParentSource(SourceInterface& source, SourceGroupInterface& group) const;
  std::shared_ptr<SourceInterface> mergeSources(SourceInterface& parent,
      const std::vector<SourceGroupInterface::iterator> children) const;

  std::shared_ptr<SourceFactory> m_source_factory;
  unsigned int m_min_area;
};

}


#endif /* _SEIMPLEMENTATION_DEBLENDING_CLEANING_H_ */

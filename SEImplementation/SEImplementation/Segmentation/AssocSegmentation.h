/*
 * AssocSegmentation.h
 *
 *  Created on: Sep 6, 2022
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_SEGMENTATION_ASSOCSEGMENTATION_H_
#define _SEIMPLEMENTATION_SEGMENTATION_ASSOCSEGMENTATION_H_

#include "SEFramework/Source/SourceFactory.h"
#include "SEFramework/Pipeline/Segmentation.h"

namespace SourceXtractor {


/**
 * @class AssocSegmentation
 * @brief Implements a Segmentation based on CNN
 */
class AssocSegmentation : public Segmentation::Labelling {
public:

  virtual ~AssocSegmentation() = default;

  AssocSegmentation(std::shared_ptr<SourceFactory> source_factory) : m_source_factory(source_factory) {
    assert(source_factory != nullptr);
  }

  void labelImage(Segmentation::LabellingListener& listener, std::shared_ptr<const DetectionImageFrame> frame) override;

private:
  std::shared_ptr<SourceFactory> m_source_factory;

};

}

#endif /* _SEIMPLEMENTATION_SEGMENTATION_ASSOCSEGMENTATION_H_ */

/*
 * FlexibleModelFittingFrame.h
 *
 *  Created on: Sep 17, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGFRAME_H_
#define _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGFRAME_H_

#include <memory>
#include <vector>

#include "SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingModel.h"

namespace SExtractor {

class FlexibleModelFittingFrame {
public:

  FlexibleModelFittingFrame(unsigned int frame_nb,
      const std::vector<std::shared_ptr<FlexibleModelFittingModel>>& models)
          : m_frame_nb(frame_nb), m_models(models) {}

  virtual ~FlexibleModelFittingFrame() {}

  unsigned int getFrameNb() const {
    return m_frame_nb;
  }

  std::vector<std::shared_ptr<FlexibleModelFittingModel>> getModels() const {
    return m_models;
  }

private:
  unsigned int m_frame_nb;
  std::vector<std::shared_ptr<FlexibleModelFittingModel>> m_models;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_FLEXIBLEMODELFITTINGFRAME_H_ */

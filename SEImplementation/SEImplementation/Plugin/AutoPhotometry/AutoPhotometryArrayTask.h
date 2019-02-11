/*
 * AutoPhotometry.h
 *
 *  Created on: Nov 23, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYARRAYTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYARRAYTASK_H_

#include "SEUtils/Types.h"
#include "SEFramework/Task/SourceTask.h"

namespace SExtractor {

/**
 * Merge all the AutoPhotometry measurements into a single vector so it can
 * be written as a vector column
 */
class AutoPhotometryArrayTask: public SourceTask {
public:

  virtual ~AutoPhotometryArrayTask() = default;

  AutoPhotometryArrayTask(const std::vector<unsigned> &images): m_images(images) {}

  virtual void computeProperties(SourceInterface& source) const override;

private:
  std::vector<unsigned> m_images;
};

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYARRAYTASK_H_

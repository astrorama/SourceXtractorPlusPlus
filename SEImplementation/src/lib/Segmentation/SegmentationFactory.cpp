/**
 * @file src/lib/SegmentationFactory.cpp
 * @date 06/08/16
 * @author mschefer
 */

#include "SEImplementation/Segmentation/Lutz.h"
#include "SEImplementation/Segmentation/SegmentationFactory.h"

namespace SEImplementation {

SegmentationFactory::SegmentationFactory(std::shared_ptr<TaskRegistry> task_registry)
: m_task_registry(task_registry) {}

std::shared_ptr<Segmentation> SegmentationFactory::getSegmentation() const {
  return std::make_shared<Lutz>(m_task_registry);
}



} // SEImplementation namespace




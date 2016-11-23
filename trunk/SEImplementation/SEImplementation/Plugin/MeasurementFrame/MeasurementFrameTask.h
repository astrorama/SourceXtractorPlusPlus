/*
 * MeasurementFrameTask.h
 *
 *  Created on: Nov 3, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAME_MEASUREMENTFRAMETASK_H_
#define _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAME_MEASUREMENTFRAMETASK_H_

#include "SEFramework/Task/SourceTask.h"

namespace SExtractor {

class MeasurementFrameTask : public SourceTask {
public:

  /**
   * @brief Destructor
   */
  virtual ~MeasurementFrameTask() = default;

  MeasurementFrameTask(unsigned int instance, std::shared_ptr<MeasurementImage> measurement_image, std::shared_ptr<CoordinateSystem> coordinate_system) :
    m_instance(instance),
    m_measurement_image(measurement_image),
    m_coordinate_system(coordinate_system) {}

  virtual void computeProperties(SourceInterface& source) const override;

private:
  unsigned int m_instance;
  std::shared_ptr<MeasurementImage> m_measurement_image;
  std::shared_ptr<CoordinateSystem> m_coordinate_system;
};

class DefaultMeasurementFrameTask : public SourceTask {
public:

  /**
   * @brief Destructor
   */
  virtual ~DefaultMeasurementFrameTask() = default;

  DefaultMeasurementFrameTask(unsigned int instance) : m_instance(instance) {}

  virtual void computeProperties(SourceInterface& source) const override;

private:
  unsigned int m_instance;
};
}

#endif

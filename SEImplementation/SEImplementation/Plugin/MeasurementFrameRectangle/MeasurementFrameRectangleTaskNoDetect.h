/*
 * MeasurementFrameRectangleTaskNoDetect.h
 *
 *  Created on: May 9, 2023
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMERECTANGLE_MEASUREMENTFRAMERECTANGLETASKNODETECT_H_
#define _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMERECTANGLE_MEASUREMENTFRAMERECTANGLETASKNODETECT_H_


#include "SEFramework/Task/SourceTask.h"

namespace SourceXtractor {

class MeasurementFrameRectangleTaskNoDetect : public SourceTask {
public:

  virtual ~MeasurementFrameRectangleTaskNoDetect() = default;

  explicit MeasurementFrameRectangleTaskNoDetect(unsigned instance): m_instance{instance} {}

  void computeProperties(SourceInterface& source) const override;

private:
  unsigned m_instance;
};

} // end SourceXtractor


#endif /* _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMERECTANGLE_MEASUREMENTFRAMERECTANGLETASKNODETECT_H_ */

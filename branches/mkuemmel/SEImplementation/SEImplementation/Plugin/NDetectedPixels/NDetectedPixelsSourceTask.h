/**
 * @file SEImplementation/NDetectedPixelsSourceTask.h
 * @date 03/14/17
 * @author mkuemmel
 */
#ifndef _SEIMPLEMENTATION_PLUGIN_NDETECTEDPIXELSSOURCETASK_H_
#define _SEIMPLEMENTATION_PLUGIN_NDETECTEDPIXELSSOURCETASK_H_

#include "SEFramework/Task/SourceTask.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
namespace SExtractor {
class NDetectedPixelsSourceTask : public SourceTask {
public:
  virtual ~NDetectedPixelsSourceTask() = default;
  virtual void computeProperties(SourceInterface& source) const {
  const auto& pixel_values = source.getProperty<DetectionFramePixelValues>().getValues();
  long int n_pixels = (long int)pixel_values.size();
  source.setProperty<NDetectedPixels>(n_pixels);
};
private:
}; // End of NDetectedPixelsSourceTask class
} // namespace SExtractor

#endif /* _SEIMPLEMENTATION_PLUGIN_NDETECTEDPIXELSSOURCETASK_H_ */

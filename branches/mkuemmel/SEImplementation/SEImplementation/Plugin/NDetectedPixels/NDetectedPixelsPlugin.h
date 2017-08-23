/**
 * @file SEImplementation/NDetectedPixelsPlugin.h
 * @date 03/14/17
 * @author mkuemmel
 */
#ifndef _SEIMPLEMENTATION_PLUGIN_NDETECTEDPIXELSPLUGIN_H_
#define _SEIMPLEMENTATION_PLUGIN_NDETECTEDPIXELSPLUGIN_H_

#include "NDetectedPixels.h"
#include "SEFramework/Plugin/Plugin.h"
#include "SEImplementation/Plugin/NDetectedPixels/NDetectedPixelsTaskFactory.h"

namespace SExtractor {
class NDetectedPixelsPlugin : public Plugin {
public:
  virtual ~NDetectedPixelsPlugin() = default;
  virtual void registerPlugin(PluginAPI& plugin_api) {
    plugin_api.getTaskFactoryRegistry().registerTaskFactory<NDetectedPixelsTaskFactory, NDetectedPixels>();
    //plugin_api.getOutputRegistry().registerColumnConverter<NDetectedPixels, int>(
    plugin_api.getOutputRegistry().registerColumnConverter<NDetectedPixels, long int>(
            "n_detected_pixels",
            [](const NDetectedPixels& prop){
              return prop.getNDetectedPixels();
            }
    );
    plugin_api.getOutputRegistry().optionalOutput<NDetectedPixels>("NDetectedPixels");
  }
  virtual std::string getIdString() const {
    return "n_detected_pixels";
  }
private:
}; // end of NDetectedPixelsPlugin class
}  // namespace SExtractor

#endif /* _SEIMPLEMENTATION_PLUGIN_NDETECTEDPIXELS_H_ */

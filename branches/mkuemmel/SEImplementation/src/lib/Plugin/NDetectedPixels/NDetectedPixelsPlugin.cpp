#include "SEFramework/Plugin/StaticPlugin.h"
#include "SEImplementation/Plugin/NDetectedPixels/NDetectedPixelsPlugin.h"
namespace SExtractor {
  static StaticPlugin<NDetectedPixelsPlugin> n_detected_pixels;
}

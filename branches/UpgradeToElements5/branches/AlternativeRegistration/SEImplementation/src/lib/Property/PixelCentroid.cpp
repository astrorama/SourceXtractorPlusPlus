/* 
 * @file PixelCentroid.cpp
 * @author nikoapos
 */

#include "SEImplementation/Property/PixelCentroid.h"
#include "SEFramework/Registration/OutputRegistry.h"

namespace SExtractor {

namespace {

struct StaticCode {
  StaticCode() {
    
    output_registry.registerColumnConverter<PixelCentroid, double>(
            "pixel_centroid_x",
            [](const PixelCentroid& prop){
              return prop.getCentroidX();
            }
    );
    
    output_registry.registerColumnConverter<PixelCentroid, double>(
            "pixel_centroid_y",
            [](const PixelCentroid& prop){
              return prop.getCentroidY();
            }
    );
    
  }
};

StaticCode static_code;

}

}
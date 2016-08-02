/* 
 * @file PixelBoundaries.cpp
 * @author nikoapos
 */

#include "SEImplementation/Property/PixelBoundaries.h"
#include "SEFramework/Registration/OutputRegistry.h"

namespace SExtractor {

namespace {

struct StaticCode {
  StaticCode() {
    
    output_registry.registerColumnConverter<PixelBoundaries, int>(
            "pixel_boundaries_min_x",
            [](const PixelBoundaries& prop){
              return prop.getMin().m_x;
            }
    );
    
    output_registry.registerColumnConverter<PixelBoundaries, int>(
            "pixel_boundaries_min_y",
            [](const PixelBoundaries& prop){
              return prop.getMin().m_y;
            }
    );
    
    output_registry.registerColumnConverter<PixelBoundaries, int>(
            "pixel_boundaries_max_x",
            [](const PixelBoundaries& prop){
              return prop.getMax().m_x;
            }
    );
    
    output_registry.registerColumnConverter<PixelBoundaries, int>(
            "pixel_boundaries_max_y",
            [](const PixelBoundaries& prop){
              return prop.getMax().m_y;
            }
    );
    
  }
};

StaticCode static_code;

}

}
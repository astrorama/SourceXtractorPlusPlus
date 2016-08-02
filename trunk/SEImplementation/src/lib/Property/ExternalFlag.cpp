/* 
 * @file ExternalFlag.cpp
 * @author nikoapos
 */

#include "SEImplementation/Property/ExternalFlag.h"
#include "SEFramework/Registration/OutputRegistry.h"

namespace SExtractor {

namespace {

struct StaticCode {
  StaticCode() {
    
    output_registry.registerColumnConverter<ExternalFlag, std::int64_t>(
            "IMAFLAGS_ISO",
            [](const ExternalFlag& prop){
              return prop.getFlag();
            }
    );
    
    output_registry.registerColumnConverter<ExternalFlag, int>(
            "NIMAFLAGS_ISO",
            [](const ExternalFlag& prop){
              return prop.getCount();
            }
    );
    
    // External flags are always in the output if there is configuration for
    // producing them
    output_registry.enableOutput<ExternalFlag>();
    
  }
};

StaticCode static_code;

}

}
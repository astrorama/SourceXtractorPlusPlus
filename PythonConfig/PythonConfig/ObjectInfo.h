/* 
 * @file ObjectInfo.h
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#ifndef _PYTHONCONFIG_OBJECTINFO_H
#define _PYTHONCONFIG_OBJECTINFO_H

#include <functional>
#include <SEUtils/Types.h>
#include <SEFramework/Source/SourceInterface.h>

namespace SExtractor {

class ObjectInfo {
  
public:
  
  ObjectInfo(SourceInterface& source);
  
  virtual ~ObjectInfo() = default;
  
  SeFloat getAlphaCoord();
  
  SeFloat getDeltaCoord();
  
  SeFloat getIsoFlux();
  
  SeFloat getRadiusWorld();
  
private:
  
  std::reference_wrapper<SourceInterface> m_source;
  
};

} // end of namespace SExtractor

#endif // _PYTHONCONFIG_OBJECTINFO_H


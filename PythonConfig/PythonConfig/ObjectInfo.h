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
  
  ObjectInfo(const SourceInterface& source);
  
  virtual ~ObjectInfo() = default;
  
  SeFloat getAlphaCoord() const;
  
  SeFloat getDeltaCoord() const;
  
  SeFloat getIsoFlux() const;
  
  SeFloat getRadiusWorld() const;
  
private:
  
  std::reference_wrapper<const SourceInterface> m_source;
  
};

} // end of namespace SExtractor

#endif // _PYTHONCONFIG_OBJECTINFO_H


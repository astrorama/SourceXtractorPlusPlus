/* 
 * @file ObjectInfo.h
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#ifndef _SEIMPLEMENTATION_OBJECTINFO_H
#define _SEIMPLEMENTATION_OBJECTINFO_H

#include <functional>
#include <SEUtils/Types.h>
#include <SEFramework/Source/SourceInterface.h>

namespace SExtractor {

class ObjectInfo {
  
public:
  
  ObjectInfo(const SourceInterface& source);
  
  virtual ~ObjectInfo() = default;
  
  SeFloat getCentroidX() const;
  
  SeFloat getCentroidY() const;
  
  SeFloat getIsoFlux() const;
  
  SeFloat getRadius() const;
  
  SeFloat getAngle() const;

  SeFloat getAspectRatio() const;

private:
  
  std::reference_wrapper<const SourceInterface> m_source;
  
};

} // end of namespace SExtractor

#endif // _SEIMPLEMENTATION_OBJECTINFO_H


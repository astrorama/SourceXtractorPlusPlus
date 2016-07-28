/* 
 * @file PixelSourceInterface.h
 * @author nikoapos
 */

#ifndef _SEFRAMEWORK_PIXELSOURCEINTERFACE_H
#define _SEFRAMEWORK_PIXELSOURCEINTERFACE_H

#include "SEUtils/PixelCoordinate.h"
#include "SEFramework/Source/SourceInterface.h"

namespace SExtractor {

class PixelSourceInterface : public SourceInterface {
  
public:
  
  PixelSourceInterface(std::vector<PixelCoordinate> pixels) 
        : m_pixels(std::move(pixels)) {
  }
  
  const std::vector<PixelCoordinate>& getPixels() {
    return m_pixels;
  }
  
private:
  
  std::vector<PixelCoordinate> m_pixels;
  
};

} /* namespace SExtractor */

#endif /* _SEFRAMEWORK_PIXELSOURCEINTERFACE_H */


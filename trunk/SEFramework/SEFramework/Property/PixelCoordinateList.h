/* 
 * @file PixelCoordinateList.h
 * @author nikoapos
 */

#ifndef _SEFRAMEWORK_PIXELCOORDINATELIST_H
#define _SEFRAMEWORK_PIXELCOORDINATELIST_H

#include "SEUtils/PixelCoordinate.h"
#include "SEFramework/Property/Property.h"

namespace SExtractor {

class PixelCoordinateList : public Property {
  
public:
  
  PixelCoordinateList(std::vector<PixelCoordinate> coordinate_list) : m_coordinate_list(coordinate_list) {
  }

  virtual ~PixelCoordinateList() = default;
  
  const std::vector<PixelCoordinate>& getCoordinateList() const {
    return m_coordinate_list;
  }
  
private:

  std::vector<PixelCoordinate> m_coordinate_list;
  
}; /* End of PixelCoordinateList class */

} /* namespace SExtractor */

#endif /* _SEFRAMEWORK_PIXELCOORDINATELIST_H */


/*
 * WCS.h
 *
 *  Created on: Nov 17, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_COORDINATESYSTEM_WCS_H_
#define _SEIMPLEMENTATION_COORDINATESYSTEM_WCS_H_

#include "SEFramework/CoordinateSystem/CoordinateSystem.h"

namespace wcslib {
struct wcsprm;
}

namespace SExtractor {

class WCS : public CoordinateSystem {
public:
  WCS(const std::string& fits_file_path);
  virtual ~WCS();

  virtual WorldCoordinate imageToWorld(ImageCoordinate image_coordinate) const;
  virtual ImageCoordinate worldToImage(WorldCoordinate world_coordinate) const;

private:
  std::unique_ptr<wcslib::wcsprm> m_wcs;
};

}

#endif /* _SEIMPLEMENTATION_COORDINATESYSTEM_WCS_H_ */

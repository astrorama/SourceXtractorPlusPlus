/*
 * PythonModule.h
 *
 *  Created on: 2019 M03 15
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PYTHONCONFIG_PYTHONMODULE_H_
#define _SEIMPLEMENTATION_PYTHONCONFIG_PYTHONMODULE_H_

#include <SEFramework/CoordinateSystem/CoordinateSystem.h>

namespace SExtractor {

  class CoordinateSystemWrapper {
  public:
    CoordinateSystemWrapper(std::shared_ptr<CoordinateSystem> coordinate_system)
      : m_coordinate_system(coordinate_system) {}

    WorldCoordinate imageToWorld(ImageCoordinate image_coordinate) const {
      return m_coordinate_system->imageToWorld(image_coordinate);
    }

    ImageCoordinate worldToImage(WorldCoordinate world_coordinate) const {
      return m_coordinate_system->worldToImage(world_coordinate);
    }

  private:
    std::shared_ptr<CoordinateSystem> m_coordinate_system;
  };

}

#endif /* _SEIMPLEMENTATION_PYTHONCONFIG_PYTHONMODULE_H_ */

/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/*
 * WCS.h
 *
 *  Created on: Nov 17, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_COORDINATESYSTEM_WCS_H_
#define _SEFRAMEWORK_COORDINATESYSTEM_WCS_H_

#include <memory>
#include <map>

#include "SEFramework/CoordinateSystem/CoordinateSystem.h"
#include "SEFramework/FITS/FitsImageSource.h"

struct wcsprm;

namespace SourceXtractor {

class WCS : public CoordinateSystem {
public:
  explicit WCS(const FitsImageSource& fits_image_source);
  explicit WCS(const WCS& original);

  virtual ~WCS();

  WorldCoordinate imageToWorld(ImageCoordinate image_coordinate) const override;
  ImageCoordinate worldToImage(WorldCoordinate world_coordinate) const override;

  std::map<std::string, std::string> getFitsHeaders() const override;

  void addOffset(PixelCoordinate pc);

private:
  void init(const char* headers, int number_of_records);

  std::unique_ptr<wcsprm, std::function<void(wcsprm*)>> m_wcs;
};

}

#endif /* _SEFRAMEWORK_COORDINATESYSTEM_WCS_H_ */

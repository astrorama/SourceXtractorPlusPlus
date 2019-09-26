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
 * Lutz.h
 *
 *  Created on: Jan 17, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_SEGMENTATION_LUTZ_H_
#define _SEIMPLEMENTATION_SEGMENTATION_LUTZ_H_

#include "ElementsKernel/Logging.h"

#include "SEFramework/Source/SourceFactory.h"
#include "SEFramework/Task/TaskProvider.h"
#include "SEFramework/Source/SourceWithOnDemandProperties.h"
#include "SEFramework/Pipeline/Segmentation.h"
#include "SEFramework/Image/Image.h"

namespace SExtractor {

class Lutz {
public:
  class PixelGroup {
  public:

    int start;
    int end;
    std::vector<PixelCoordinate> pixel_list;

    PixelGroup() : start(-1), end(-1) {}

    void merge_pixel_list(PixelGroup& other) {
      pixel_list.insert(pixel_list.end(), other.pixel_list.begin(), other.pixel_list.end());
    }
  };

  class LutzListener {
  public:
    virtual void publishGroup(PixelGroup& pixel_group) = 0;
    virtual void notifyProgress(int /*line*/, int /*total*/) {};
  };

  Lutz() {}
  virtual ~Lutz() = default;

  void labelImage(LutzListener& listener, const DetectionImage& image, PixelCoordinate offset = PixelCoordinate(0,0));
};

class LutzList : public Lutz, public Lutz::LutzListener {
public:

  LutzList() {}
  virtual ~LutzList() = default;

  const std::vector<PixelGroup>& getGroups() const {
    return m_groups;
  }

  void labelImage(const DetectionImage& image, PixelCoordinate offset = PixelCoordinate(0,0)) {
    Lutz::labelImage(*this, image, offset);
  }

protected:
  virtual void publishGroup(PixelGroup& pixel_group) override;

private:
  std::vector<PixelGroup> m_groups;

};

}




#endif /* _SEIMPLEMENTATION_SEGMENTATION_LUTZ_H_ */

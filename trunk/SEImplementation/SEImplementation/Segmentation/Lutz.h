/*
 * Lutz.h
 *
 *  Created on: Jan 17, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_SEGMENTATION_LUTZ_H_
#define _SEIMPLEMENTATION_SEGMENTATION_LUTZ_H_

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

  Lutz() {}
  virtual ~Lutz() = default;

  void labelImage(const DetectionImage& image);

protected:
  virtual void publishGroup(PixelGroup& pixel_group) = 0;

};

class LutzList : public Lutz {
public:

  LutzList() {}
  virtual ~LutzList() = default;

  const std::vector<PixelGroup>& getGroups() const {
    return m_groups;
  }

protected:
  virtual void publishGroup(PixelGroup& pixel_group) override;

private:
  std::vector<PixelGroup> m_groups;

};

}




#endif /* _SEIMPLEMENTATION_SEGMENTATION_LUTZ_H_ */

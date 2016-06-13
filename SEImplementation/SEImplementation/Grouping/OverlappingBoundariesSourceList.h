/**
 * @file SEImplementation/Grouping/OverlappingBoundariesSourceList.h
 * @date 06/08/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_GROUPING_OVERLAPPINGBOUNDARIESSOURCELIST_H
#define _SEIMPLEMENTATION_GROUPING_OVERLAPPINGBOUNDARIESSOURCELIST_H

#include "SEFramework/Source/SourceList.h"

namespace SExtractor {

/**
 * @class OverlappingBoundariesSourceList
 * @brief
 *
 */
class OverlappingBoundariesSourceList : public SourceList {
public:

  OverlappingBoundariesSourceList(const std::vector<std::shared_ptr<Source>>& sources = {});
  virtual ~OverlappingBoundariesSourceList() = default;

  virtual void addSource(std::shared_ptr<Source> source) override;
  virtual void removeSource(std::shared_ptr<Source> source) override;
  virtual void merge(SourceList& source_list) override;

  PixelCoordinate getMin() const {
    return m_min_coord;
  }

  PixelCoordinate getMax() const {
    return m_max_coord;
  }

private:
  PixelCoordinate m_min_coord;
  PixelCoordinate m_max_coord;

  void processSource(Source& source);

}; /* End of OverlappingBoundariesSourceList class */

} /* namespace SEImplementation */


#endif

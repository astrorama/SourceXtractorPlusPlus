/**
 * @file SEImplementation/Grouping/OverlappingBoundariesSourceList.h
 * @date 06/08/16
 * @author mschefer
 */

#ifndef _SEIMPLEMENTATION_GROUPING_OVERLAPPINGBOUNDARIESSOURCELIST_H
#define _SEIMPLEMENTATION_GROUPING_OVERLAPPINGBOUNDARIESSOURCELIST_H

#include "SEFramework/Source/SourceList.h"

namespace SEImplementation {

using SEFramework::SourceList;
using SEFramework::Source;

/**
 * @class OverlappingBoundariesSourceList
 * @brief
 *
 */
class OverlappingBoundariesSourceList : public SourceList {
public:

  OverlappingBoundariesSourceList(std::vector<std::shared_ptr<Source>> sources = {});
  virtual ~OverlappingBoundariesSourceList() = default;

  virtual void addSource(std::shared_ptr<Source> source) override;
  virtual void removeSource(std::shared_ptr<Source> source) override;
  virtual void merge(SourceList& source_list) override;

  SEUtils::PixelCoordinate getMin() const {
    return m_min_coord;
  }

  SEUtils::PixelCoordinate getMax() const {
    return m_max_coord;
  }

private:
  SEUtils::PixelCoordinate m_min_coord;
  SEUtils::PixelCoordinate m_max_coord;

  void processSource(Source& source);

}; /* End of OverlappingBoundariesSourceList class */

} /* namespace SEImplementation */


#endif

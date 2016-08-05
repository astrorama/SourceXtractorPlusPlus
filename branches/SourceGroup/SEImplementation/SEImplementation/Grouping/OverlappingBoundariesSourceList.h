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
 * @brief A specialization of SourceList that keeps track of its
 *  overall bounding box to be used with OverlappingBoundariesCriteria.
 *
 */
class OverlappingBoundariesSourceList : public SourceList {
public:

  /// Constructor
  OverlappingBoundariesSourceList(const std::vector<std::shared_ptr<Source>>& sources = {});

  /// Destructor
  virtual ~OverlappingBoundariesSourceList() = default;

  // Overrides SourceList methods
  virtual void addSource(std::shared_ptr<Source> source) override;
  virtual void removeSource(std::shared_ptr<Source> source) override;
  virtual void merge(const SourceList& source_list) override;

  /// Returns the minimum coordinate of the bounding box
  PixelCoordinate getMin() const {
    return m_min_coord;
  }

  /// Returns the maximum coordinate of the bounding box (inclusive)
  PixelCoordinate getMax() const {
    return m_max_coord;
  }

private:
  PixelCoordinate m_min_coord;
  PixelCoordinate m_max_coord;

  /// Helper method to update the bounding box by adding one source
  void processSource(const Source& source);

}; /* End of OverlappingBoundariesSourceList class */

} /* namespace SExtractor */


#endif

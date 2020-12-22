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

#ifndef _SEIMPLEMENTATION_SEGMENTATION_BFSSEGMENTATION_H_
#define _SEIMPLEMENTATION_SEGMENTATION_BFSSEGMENTATION_H_

#include "SEFramework/Source/SourceFactory.h"
#include "SEFramework/Pipeline/Segmentation.h"

namespace SourceXtractor {


/**
 * @class BFSSegmentation
 * @brief Implements a Segmentation based on the BFS algorithm
 */
class BFSSegmentation : public Segmentation::Labelling {
public:

  virtual ~BFSSegmentation() = default;

  BFSSegmentation(std::shared_ptr<SourceFactory> source_factory, int max_delta)
      : m_source_factory(source_factory), m_max_delta(max_delta) {
    assert(source_factory != nullptr);
  }

  void labelImage(Segmentation::LabellingListener& listener, std::shared_ptr<const DetectionImageFrame> frame) override;

private:
  class VisitedMap {
  public:
    VisitedMap(int width, int height) : m_width(width), m_height(height), m_visited(width * height, false) {}

    void markVisited(PixelCoordinate pc) {
      m_visited[pc.m_x + pc.m_y * m_width] = true;
    }

    bool wasVisited(PixelCoordinate pc) const {
      if (pc.m_x >= 0 && pc.m_x < m_width && pc.m_y >= 0 && pc.m_y < m_height) {
        return m_visited[pc.m_x + pc.m_y * m_width];
      } else {
        return true;
      }
    }

  private:
    int m_width, m_height;
    std::vector<bool> m_visited;
  };

  struct Tile {    PixelCoordinate offset;
    int width, height;
  };

  void labelSource(PixelCoordinate pc, Segmentation::LabellingListener& listener,
                   DetectionImage& detection_image, VisitedMap& visited_map) const;

  std::vector<BFSSegmentation::Tile> getTiles(const DetectionImage& image) const;


  std::shared_ptr<SourceFactory> m_source_factory;
  int m_max_delta;
};

}

#endif /* _SEIMPLEMENTATION_SEGMENTATION_BFSSEGMENTATION_H_ */

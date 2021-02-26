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


#ifndef _SEIMPLEMENTATION_SEGMENTATION_MLSEGMENTATION_H_
#define _SEIMPLEMENTATION_SEGMENTATION_MLSEGMENTATION_H_

#include "SEFramework/Source/SourceFactory.h"
#include "SEFramework/Pipeline/Segmentation.h"

namespace SourceXtractor {


/**
 * @class MLSegmentation
 * @brief Implements a Segmentation based on the BFS algorithm
 */
class MLSegmentation : public Segmentation::Labelling {
public:

  virtual ~MLSegmentation() = default;

  MLSegmentation(std::shared_ptr<SourceFactory> source_factory, std::string model_path)
      : m_source_factory(source_factory), m_model_path(model_path) {
    assert(source_factory != nullptr);
  }

  void labelImage(Segmentation::LabellingListener& listener, std::shared_ptr<const DetectionImageFrame> frame) override;

private:
  struct Tile {
    PixelCoordinate offset;
    int width, height;
  };

  std::vector<MLSegmentation::Tile> getTiles(const DetectionImage& image) const;


  std::shared_ptr<SourceFactory> m_source_factory;

  std::string m_model_path;
};

}

#endif /* _SEIMPLEMENTATION_SEGMENTATION_MLSEGMENTATION_H_ */

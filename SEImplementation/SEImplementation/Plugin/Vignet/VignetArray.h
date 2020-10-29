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

#ifndef _SEIMPLEMENTATION_PLUGIN_VIGNETARRAY_H_
#define _SEIMPLEMENTATION_PLUGIN_VIGNETARRAY_H_

#include <NdArray/NdArray.h>
#include <AlexandriaKernel/memory_tools.h>
#include "SEFramework/Image/Image.h"
#include "SEFramework/Property/Property.h"
#include "SEImplementation/Plugin/Vignet/Vignet.h"

namespace SourceXtractor {

class VignetArray: public Property {
public:
  VignetArray(const std::vector<std::reference_wrapper<const Vignet>>& vignets) {
    const auto& representative = vignets.front().get().getVignet();

    if (vignets.size() > 1) {
      initMultiple(vignets, representative);
    }
    else {
      initSingle(representative);
    }
  }

  const Euclid::NdArray::NdArray<DetectionImage::PixelType>& getVignets() const {
    return *m_vignets;
  }

private:
  void initSingle(const VectorImage <DetectionImage::PixelType>& representative) {
    std::vector<size_t> shape{
      static_cast<size_t>(representative.getWidth()), static_cast<size_t>(representative.getHeight())
    };
    m_vignets = Euclid::make_unique<Euclid::NdArray::NdArray<DetectionImage::PixelType>>(shape);

    for (int x = 0; x < representative.getWidth(); ++x) {
      for (int y = 0; y < representative.getHeight(); ++y) {
        m_vignets->at(x, y) = representative.getValue(x, y);
      }
    }
  }

  void initMultiple(const std::vector<std::reference_wrapper<const Vignet>>& vignets,
                    const VectorImage <DetectionImage::PixelType>& representative) {
    std::vector<size_t> shape{
      vignets.size(), static_cast<size_t>(representative.getWidth()), static_cast<size_t>(representative.getHeight())
    };
    m_vignets = Euclid::make_unique<Euclid::NdArray::NdArray<DetectionImage::PixelType>>(shape);

    for (size_t v = 0; v < vignets.size(); ++v) {
      const auto& img = vignets[v].get().getVignet();
      assert (img.getWidth() == representative.getWidth());
      assert (img.getHeight() == representative.getHeight());
      for (int x = 0; x < img.getWidth(); ++x) {
        for (int y = 0; y < img.getHeight(); ++y) {
          m_vignets->at(v, x, y) = img.getValue(x, y);
        }
      }
    }
  }

  std::unique_ptr<Euclid::NdArray::NdArray<DetectionImage::PixelType>> m_vignets;
};

} // end of namespace SourceXtractor

#endif /* _SEIMPLEMENTATION_PLUGIN_VIGNETARRAY_H_ */

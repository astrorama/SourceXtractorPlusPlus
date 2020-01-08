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

#ifndef _SEUTILS_HILBERTCURVE_H_
#define _SEUTILS_HILBERTCURVE_H_

#include <vector>

#include "SEUtils/Misc.h"
#include "SEUtils/PixelCoordinate.h"

namespace SourceXtractor {

class HilbertCurve {
public:
  HilbertCurve(unsigned int size) {
    m_size = nextPowerOfTwo(size);
  }

  std::vector<PixelCoordinate> getCurve() const {
    std::vector<PixelCoordinate> hilbert_curve;
    for (unsigned int i = 0; i < m_size * m_size; i++) {
      hilbert_curve.emplace_back(get2DCoordinate(i));
    }
    return hilbert_curve;
  }

  PixelCoordinate get2DCoordinate(unsigned int index) const {
    unsigned int x = 0;
    unsigned int y = 0;
    for (unsigned int s=1; s<m_size; s*=2) {
      unsigned int rx = 1 & (index >> 1);
      unsigned int ry = 1 & (index ^ rx);

      if (ry == 0) {
        if (rx == 1) {
          x = s-1 - x;
          y = s-1 - y;
        }

        std::swap(x, y);
      }

      x += s * rx;
      y += s * ry;
      index >>= 2;
    }

    return {(int) x, (int) y};
  }

private:
  unsigned int m_size;
};

}

#endif /* _SEUTILS_HILBERTCURVE_H_ */

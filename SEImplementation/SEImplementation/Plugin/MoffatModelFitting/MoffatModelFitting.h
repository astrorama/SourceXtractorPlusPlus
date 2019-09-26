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
 * MoffatModelFitting.h
 *
 *  Created on: May 2, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MOFFATMODELFITTING_MOFFATMODELFITTING_H_
#define _SEIMPLEMENTATION_PLUGIN_MOFFATMODELFITTING_MOFFATMODELFITTING_H_

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"

namespace SExtractor {

class MoffatModelFitting : public Property {
public:

  virtual ~MoffatModelFitting() = default;

  MoffatModelFitting(
      SeFloat x,
      SeFloat y,
      SeFloat moffat_i0,
      SeFloat moffat_index,
      SeFloat minkowski_exponent,
      SeFloat top_offset,
      SeFloat size,
      SeFloat x_scale,
      SeFloat y_scale,
      SeFloat moffat_rotation,
      unsigned int iterations) :
    m_x(x),
    m_y(y),
    m_moffat_i0(moffat_i0),
    m_moffat_index(moffat_index),
    m_minkowski_exponent(minkowski_exponent),
    m_top_offset(top_offset),
    m_size(size),
    m_x_scale(x_scale),
    m_y_scale(y_scale),
    m_moffat_rotation(moffat_rotation),
    m_iterations(iterations) {}

  SeFloat getX() const {
    return m_x;
  }

  SeFloat getY() const {
    return m_y;
  }

  SeFloat getMoffatI0() const {
    return m_moffat_i0;
  }

  SeFloat getMoffatIndex() const {
    return m_moffat_index;
  }

  SeFloat getMinkowksiExponent() const {
    return m_minkowski_exponent;
  }

  SeFloat getTopOffset() const {
    return m_top_offset;
  }

  SeFloat getSize() const {
    return m_size;
  }

  SeFloat getXScale() const {
    return m_x_scale;
  }

  SeFloat getYScale() const {
    return m_y_scale;
  }

  SeFloat getMoffatRotation() const {
    return m_moffat_rotation;
  }

  unsigned int getIterations() const {
    return m_iterations;
  }

private:
  SeFloat m_x, m_y;
  SeFloat m_moffat_i0;
  SeFloat m_moffat_index;
  SeFloat m_minkowski_exponent;
  SeFloat m_top_offset;
  SeFloat m_size;
  SeFloat m_x_scale;
  SeFloat m_y_scale;
  SeFloat m_moffat_rotation;

  unsigned int m_iterations;
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_MOFFATMODELFITTING_MOFFATMODELFITTING_H_ */

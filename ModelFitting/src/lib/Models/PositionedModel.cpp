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
/** 
 * @file PositionedModel.cpp
 * @date September 1, 2015
 * @author Nikolaos Apostolakos
 */

#include "ModelFitting/Models/PositionedModel.h"

namespace ModelFitting {

PositionedModel::PositionedModel(BasicParameter& x, BasicParameter& y)
        : m_x {x.getValue()}, m_y{y.getValue()},
          m_x_updater{x, m_x}, m_y_updater{y, m_y}
{
}

PositionedModel::PositionedModel(PositionedModel&& other)
        : m_x {other.m_x}, m_y{other.m_y},
          m_x_updater{other.m_x_updater.getParameter(), m_x},
          m_y_updater{other.m_y_updater.getParameter(), m_y}
{
}

double PositionedModel::getX() const {
  return m_x;
}

double PositionedModel::getY() const {
  return m_y;
}

} // end of namespace ModelFitting
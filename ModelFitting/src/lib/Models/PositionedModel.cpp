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
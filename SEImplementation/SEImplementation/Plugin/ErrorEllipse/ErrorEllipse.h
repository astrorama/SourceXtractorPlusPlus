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
 * ErrorEllipse.h
 *
 *  Created on: Feb 11, 2022
 *      Author: mkuemmel
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_ERRORELLIPSE_ERRORELLIPSE_H_
#define _SEIMPLEMENTATION_PLUGIN_ERRORELLIPSE_ERRORELLIPSE_H_

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"

namespace SourceXtractor {

class ErrorEllipse : public Property {
public:

	  ErrorEllipse(SeFloat a_error, SeFloat b_error, SeFloat theta_error, SeFloat cxx_error, SeFloat cyy_error, SeFloat cxy_error)
	    : m_a_error(a_error), m_b_error(b_error), m_theta_error(theta_error), m_cxx_error(cxx_error), m_cyy_error(cyy_error), m_cxy_error(cxy_error) {}

  virtual ~ErrorEllipse() = default;

  /**
   * Semi-major axis error
   */
  SeFloat getEllipseAError() const {
    return m_a_error;
  }

  /**
   * Semi-minor axis error
   */
  SeFloat getEllipseBError() const {
    return m_b_error;
  }

  /**
   * Angle error in degrees
   */
  SeFloat getEllipseThetaError() const {
    return m_theta_error;
  }

  SeFloat getEllipseCxxError() const {
    return m_cxx_error;
  }

  SeFloat getEllipseCyyError() const {
    return m_cyy_error;
  }

  SeFloat getEllipseCxyError() const {
    return m_cxy_error;
  }

private:
  SeFloat m_a_error, m_b_error, m_theta_error; // errors for ellipse semi-major axis, semi-minor axis, and angle
  SeFloat m_cxx_error, m_cyy_error, m_cxy_error;

};

}
#endif /* _SEIMPLEMENTATION_PLUGIN_ERRORELLIPSE_ERRORELLIPSE_H_ */

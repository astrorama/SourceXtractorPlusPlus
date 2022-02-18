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
 * ShapeParameters.h
 *
 *  Created on: Jan 27, 2017
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_SHAPEPARAMETERS_SHAPEPARAMETERS_H_
#define _SEIMPLEMENTATION_PLUGIN_SHAPEPARAMETERS_SHAPEPARAMETERS_H_

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"

namespace SourceXtractor {

class ShapeParameters : public Property {
public:

	  ShapeParameters(SeFloat a, SeFloat b, SeFloat theta, SeFloat abcor, SeFloat cxx, SeFloat cyy, SeFloat cxy, SeFloat area, SeFloat intensity, bool singu)
	    : m_a(a), m_b(b), m_theta(theta), m_abcor(abcor), m_cxx(cxx), m_cyy(cyy), m_cxy(cxy), m_area(area), m_intensity(intensity), m_singu_flag(singu) {}

  virtual ~ShapeParameters() = default;

  /**
   * Semi-major axis
   */
  SeFloat getEllipseA() const {
    return m_a;
  }

  /**
   * Semi-minor axis
   */
  SeFloat getEllipseB() const {
    return m_b;
  }

  /**
   * Angle in degrees
   */
  SeFloat getEllipseTheta() const {
    return m_theta;
  }

  /**
   * \f[
   *    \frac{\cos^2 {\tt THETA}}{{\tt A}^2} + \frac{\sin^2 {\tt THETA}}{{\tt B}^2} =
   *    \frac{\overline{y^2}}{\overline{x^2} \overline{y^2} - \overline{xy}^2}
   * \f]
   */
  SeFloat getEllipseCxx() const {
    return m_cxx;
  }

  /**
   * \f[
   *     \frac{\sin^2 {\tt THETA}}{{\tt A}^2} + \frac{\cos^2 {\tt THETA}}{{\tt B}^2} =
   *     \frac{\overline{x^2}}{\overline{x^2} \overline{y^2} - \overline{xy}^2}
   * \f]
   */
  SeFloat getEllipseCyy() const {
    return m_cyy;
  }

  /**
   * \f[
   *     2 \,\cos {\tt THETA}\,\sin {\tt THETA} \left( \frac{1}{{\tt A}^2} - \frac{1}{{\tt B}^2}\right) =
   *     -2\,\frac{\overline{xy}}{\overline{x^2} \overline{y^2} - \overline{xy}^2}
   * \f]
   */
  SeFloat getEllipseCxy() const {
    return m_cxy;
  }

  SeFloat getAbcor() const {
    return m_abcor;
  }

  SeFloat getArea() const {
    return m_area;
  }

  SeFloat getElongation() const {
    return m_a / m_b;
  }

  SeFloat getEllipticity() const {
    return 1 - m_b / m_a;
  }

  SeFloat getIntensity() const {
    return m_intensity;
  }

  bool getSinguFlag() const {
    return m_singu_flag;
  }

private:
  SeFloat m_a, m_b, m_theta; // ellipse semi-major axis, semi-minor axis, and angle
  SeFloat m_abcor;
  SeFloat m_cxx, m_cyy, m_cxy;
  SeFloat m_area;
  SeFloat m_intensity;
  bool m_singu_flag;
};

}



#endif /* _SEIMPLEMENTATION_PLUGIN_SHAPEPARAMETERS_SHAPEPARAMETERS_H_ */

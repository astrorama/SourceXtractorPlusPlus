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
 * VariablePsf.h
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro Álvarez Ayllón
 */

#ifndef _SEIMPLEMENTATION_PSF_VARIABLEPSF_H_
#define _SEIMPLEMENTATION_PSF_VARIABLEPSF_H_

#include <SEFramework/Image/VectorImage.h>
#include "SEFramework/Property/PropertyHolder.h"

namespace SourceXtractor {


/**
 * @class VariablePsf
 *
 * @brief
 * Implements a variable PSF using an arbitrary number of components (i.e. X, Y), and degrees.
 *
 * @details
 * It is based on SExtractor logic, so based on a polynomial where the variables are some attributes
 * of a source. For instance, if the components were X and Y, both in the same group, and the degree 2,
 * the polynomial would be
 *
 *      \f$C + X + X^2 + XY + Y + Y^2\f$
 *
 * The coefficients must be given on that order (note that the constant would be the first element)
 *
 */
class VariablePsf {
public:

  /**
   * A component belongs to one single group, and is scaled before
   * being used:
   *
   * \f$x_i = (V_i - offset) / scale \f$
   */
  struct Component {
    std::string name;
    int group_id;
    double offset, scale;
  };

  /**
   * Constructor
   * @param pixel_sampling
   *    Unused by the class itself, it is an attribute of the PSF.
   *    It is the sampling step size of the PSF on image pixels. i.e. a value of 0.5 means that
   *    a PSF pixel has been sampled every 0.5 pixels on the corresponding image.
   * @param components
   *    List of components (or variables) to be used by the Variable PSF
   * @param group_degrees
   *    Polynomial degree. Each group has its own degree, so there has to be as many as different group_id
   *    there are on the components
   */
  VariablePsf(double pixel_sampling, const std::vector<Component> &components, const std::vector<int> &group_degrees,
    const std::vector<std::shared_ptr<VectorImage<SeFloat>>> &coefficients);

  /**
   * Convenience constructor that initializes the variable PSF with just a constant value
   * (So it is not variable anymore)
   */
  VariablePsf(double pixel_sampling, const std::shared_ptr<VectorImage<SeFloat>> &constant);

  /**
   * Destructor
   */
  virtual ~VariablePsf() = default;

  /**
   * @return The width of the PSF
   */
  virtual int getWidth() const;

  /**
   * @return The height of the PSF
   */
  virtual int getHeight() const;

  /**
   * @return The pixel sampling
   */
  virtual double getPixelSampling() const;

  /**
   * @return A reference to the list of components
   */
  virtual const std::vector<Component>& getComponents() const;

  /**
   * Reconstructs a PSF based on the given values for each of the component.
   * @param values
   *    Component values. Note that they have to be in the same order (and as many)
   *    as components were passed to the constructor (none for constant PSF).
   * @return
   *    The reconstructed PSF
   * @throws
   *    If the number of values does not match the number of components
   */
  virtual std::shared_ptr<VectorImage<SeFloat>> getPsf(const std::vector<double> &values) const;

private:
  double m_pixel_sampling;
  std::vector<Component> m_components;
  std::vector<int> m_group_degrees;
  std::vector<std::shared_ptr<VectorImage<SeFloat>>> m_coefficients;
  std::vector<std::vector<int>> m_exponents;

  /// Verify that the preconditions of getPsf are met at construction time
  virtual void selfTest();

  /// Normalizes the values
  std::vector<double> scaleProperties(const std::vector<double> &values) const;

  /**
   * Calculates the exponents for each component per term of the polynomial.
   * @details
   *    For instance, for (X, Y) degree 2, this would generate the matrix
   *    \code{.unparsed}
   *    [0, 0] // constant
   *    [1, 0] // X
   *    [2, 0] // X^2
   *    [1, 1] // XY
   *    [0, 1] // Y
   *    [0, 2] // Y^2
   *    \endcode
   */
  void calculateExponents();
};

}

#endif //_SEIMPLEMENTATION_PSF_VARIABLEPSF_H_

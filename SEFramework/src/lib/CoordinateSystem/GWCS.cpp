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
 * GWCS.cpp
 *
 *  Created on: Sep 23, 2026
 *      Author: embray
 */

#include "SEFramework/CoordinateSystem/GWCS.h"

#include <cmath>

#include "ElementsKernel/Exception.h"

namespace SourceXtractor {

namespace {

/**
 * Bring a longitude into [0, 360)
 *
 * AST returns longitudes in (-180, 180], while wcslib and the rest of
 * SourceXtractor use [0, 360); without this, RA from a GWCS would differ from
 * the same RA from a FITS WCS by 360 degrees over half the sky.
 */
double normalizeLongitude(double alpha) {
  if (!std::isfinite(alpha)) {
    return alpha;
  }

  alpha = std::fmod(alpha, 360.0);
  return alpha < 0.0 ? alpha + 360.0 : alpha;
}

}  // anonymous namespace


GWCS::GWCS(EvalPtr eval) : m_eval(std::move(eval)) {
  if (!m_eval) {
    throw Elements::Exception() << "Cannot create a GWCS without an evaluation context";
  }
}


GWCS::~GWCS() {}


WorldCoordinate GWCS::imageToWorld(ImageCoordinate image_coordinate) const {
  double x_in = image_coordinate.m_x;
  double y_in = image_coordinate.m_y;
  double alpha = 0.0;
  double delta = 0.0;

  // NOTE: evaluating one point at a time is wasteful--libasdf-gwcs is much
  // happier with batches--but the CoordinateSystem interface is scalar.
  asdf_gwcs_err_t err = asdf_gwcs_eval_2d(m_eval.get(), &x_in, &y_in, &alpha, &delta, 1);

  if (ASDF_GWCS_OK != err) {
    throw InvalidCoordinatesException() << "Failed to evaluate the WCS at image coordinates ("
      << image_coordinate.m_x << ", " << image_coordinate.m_y << "): error " << err;
  }

  // Don't forget to normalize the longitudinal pole
  return WorldCoordinate(normalizeLongitude(alpha), delta);
}


ImageCoordinate GWCS::worldToImage(WorldCoordinate world_coordinate) const {
  throw InvalidCoordinatesException()
    << "Converting world to image coordinates is not supported for a GWCS (requested "
    << world_coordinate.m_alpha << ", " << world_coordinate.m_delta << "); libasdf-gwcs "
    << "evaluates the forward transform only";
}

}  // namespace SourceXtractor

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
 * @file SersicProfile.h
 * @date August 25, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_SERSICPROFILEPROJECTION_H
#define	MODELFITTING_SERSICPROFILEPROJECTION_H

#include "ModelFitting/Parameters/BasicParameter.h"
#include "ModelFitting/Parameters/ReferenceUpdater.h"

namespace ModelFitting {

/**
 * Mathematical function that describes how the intensity I of a galaxy varies with distance R.
 *
 * \f$ I=I_0 * e ^ {-k * R^{1 / n}} \f$
 */
class SersicProfile {
  
public:

  /**
   * Constructor
   * @param i0
   *    Intensity at the center
   * @param n
   *    Sersic index. n=1 is exponential and n=4 de Vaucouleurs
   * @param k
   *    A constant that multiplies the term \f$ R^{1 / n} \f$
   */
  SersicProfile(BasicParameter& i0, BasicParameter& n, BasicParameter& k);

  /**
   * Copy constructor
   */
  SersicProfile(const SersicProfile& other);

  /**
   * Destructor
   */
  virtual ~SersicProfile();

  /**
   * Evaluate the Sersic profile at the given distance
   * @param r
   *    The distance from the origin
   */
  double operator()(double r) const;
  
private:
  
  double m_i0;
  double m_n;
  double m_k;
  ReferenceUpdater m_i0_updater;
  ReferenceUpdater m_n_updater;
  ReferenceUpdater m_k_updater;
  
}; // end of class SersicProfile

} // end of namespace ModelFitting

#endif	/* MODELFITTING_SERSICPROFILEPROJECTION_H */


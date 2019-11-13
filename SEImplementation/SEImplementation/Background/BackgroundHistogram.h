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
 * Created on Jan 05, 2015
 * @author: mkuemmel@usm.lmu.de
 *
 * Date:     $Date$
 * Revision: $Revision$
 * Author:   $Author$
 */
#ifndef BACKGROUNDHISTOGRAM_H
#define	BACKGROUNDHISTOGRAM_H

#include <cstddef>

namespace SExtractor {

class BackgroundHistogram {

public:
  BackgroundHistogram(const double& mean, const double& sigm, const size_t& ndata);
  virtual ~BackgroundHistogram();

  //void fillInData(const PIXTYPE* cellData, const size_t ndata);
  void addDatum(const PIXTYPE& pixVal);
  
  void getBackGuessMod(PIXTYPE& bckVal, PIXTYPE& sigmaVal);
  void getBackGuess(PIXTYPE& bckVal, PIXTYPE& sigmaVal);

  double itsMean=0.0;
  double itsSigma=0.0;

private:
  size_t itsStatNData=0;

  float itsQzero  = 0.0;
  float itsQscale = 0.0;
  float itsCste   = 0.0;

  int* itsHisto=NULL;
  size_t itsNLevels=0;
};

} // end of namespace SExtractor


#endif	/* BACKGROUNDHISTOGRAM_H */


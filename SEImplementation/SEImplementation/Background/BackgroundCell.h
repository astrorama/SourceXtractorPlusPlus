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
#ifndef BACKGROUNDCELL_H
#define	BACKGROUNDCELL_H

#include "SEImplementation/Background/BackgroundHistogram.h"

namespace SExtractor {

class BackgroundCell {

public:

  BackgroundCell(const PIXTYPE* cellData, const size_t ndata,  const PIXTYPE* cellWeight=NULL, const PIXTYPE weightThresh=BIG);
  virtual ~BackgroundCell();

  void run();
  void getBackgroundValues(PIXTYPE& meanVal, PIXTYPE& sigmaVal);
  void getBackgroundValues(PIXTYPE& meanVal, PIXTYPE& sigmaVal, PIXTYPE& whtMeanVal, PIXTYPE& whtSigmaVal);
  void getBackgroundValuesOld(PIXTYPE& meanVal, PIXTYPE& sigmaVal);

private:

  void getStats(const PIXTYPE* cellData, const size_t& ndata, double& mean, double& sigma, size_t& statNData);
  void getStatsWeight(const PIXTYPE* cellData, const size_t& ndata, const PIXTYPE* cellWeight, const PIXTYPE weightThresh, double& mean, double& sigma, size_t& statNData, double& weightMean, double& weightSigma, size_t& statNWeight);

  const PIXTYPE* itsCellData;
  const PIXTYPE* itsCellWeight;
  
 
  size_t itsNdata=0;

  BackgroundHistogram* itsHisto = NULL;
  BackgroundHistogram* itsWeightHisto = NULL;

  bool itsHasWeight=false;
  PIXTYPE itsWeightThresh=BIG;

  double itsMean=0.0;
  double itsSigma=0.0;
  double itsWeightMean=0.0;
  double itsWeightSigma=0.0;
  size_t itsStatNData=0;
  size_t itsStatNWeight=0;
};
} // end of the namespace SExtractor
#endif	/* BACKGROUNDCELL_H */


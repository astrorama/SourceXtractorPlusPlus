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

#endif	/* BACKGROUNDCELL_H */


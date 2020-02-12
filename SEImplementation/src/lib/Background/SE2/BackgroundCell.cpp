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
#include <cmath>
#include <limits>
#include "ElementsKernel/Exception.h"       // for Elements Exception
#include "SEImplementation/Background/SE2/BackgroundDefine.h"
#include "SEImplementation/Background/SE2/BackgroundHistogram.h"
#include "SEImplementation/Background/SE2/BackgroundCell.h"

namespace SourceXtractor {

BackgroundCell::BackgroundCell(const PIXTYPE* cellData, const size_t ndata, const PIXTYPE* cellWeight, const PIXTYPE weightThresh)
{
  itsCellData=cellData;
  itsCellWeight=cellWeight;
  itsNdata=ndata;
  itsWeightThresh=weightThresh;

  // compute some basic statistics on the
  // cell data and weights: mean, sigma (with a clipping), npix
  if (cellWeight){
    getStatsWeight(itsCellData, itsNdata, itsCellWeight, weightThresh, itsMean, itsSigma, itsStatNData, itsWeightMean, itsWeightSigma, itsStatNWeight);
    itsHasWeight=true;
    itsHisto       = new BackgroundHistogram(itsMean, itsSigma, itsStatNData);
    itsWeightHisto = new BackgroundHistogram(itsWeightMean, itsWeightSigma, itsStatNWeight);
  }
  else{
    getStats(itsCellData, itsNdata, itsMean, itsSigma, itsStatNData);
    itsHasWeight=false;
    itsHisto = new BackgroundHistogram(itsMean, itsSigma, itsStatNData);
  }
}

BackgroundCell::~BackgroundCell()
{
  delete itsHisto;
  itsHisto=NULL;
  delete itsWeightHisto;
  itsWeightHisto=NULL;
}

void BackgroundCell::getBackgroundValues(PIXTYPE& meanVal, PIXTYPE& sigmaVal)
{
  PIXTYPE pixVal=0.0;

  meanVal  = itsHisto->itsMean;
  sigmaVal = itsHisto->itsSigma;

  // check if something can be done at all
  if (itsHisto->itsMean<=-BIG && itsHisto->itsSigma<=-BIG){
    return;
  }

  // go over the data
  for (size_t index=0; index<itsNdata; index++) {

    // take the current value
    pixVal = itsCellData[index];

    // add to the histogram
    itsHisto->addDatum(pixVal);
  }

  // determine the mean value and sigma
  itsHisto->getBackGuess(meanVal, sigmaVal);

  return;
}

void BackgroundCell::getBackgroundValues(PIXTYPE& meanVal, PIXTYPE& sigmaVal, PIXTYPE& whtMeanVal, PIXTYPE& whtSigmaVal)
{
  PIXTYPE pixVal=0.0;
  PIXTYPE whtVal=0.0;

  // transfer all mean ans sigma values
  meanVal  = itsHisto->itsMean;
  sigmaVal = itsHisto->itsSigma;
  if (itsHasWeight){
    whtMeanVal  = itsWeightHisto->itsMean;
    whtSigmaVal = itsWeightHisto->itsSigma;
  }

  // check if something can be done at all
  if (itsHisto->itsMean<=-BIG && itsHisto->itsSigma<=-BIG){
    return;
  }

  if (itsHasWeight){

    // go over the data
    for (size_t index=0; index<itsNdata; index++) {

      // take the current values
      pixVal = itsCellData[index];
      whtVal = itsCellWeight[index];
      if (whtVal<itsWeightThresh){
        // add to the histograms
        itsHisto->addDatum(pixVal);
        itsWeightHisto->addDatum(whtVal);
     }
    }
  }
  else {
    // go over the data
    for (size_t index=0; index<itsNdata; index++) {

      // take the current value
      pixVal = itsCellData[index];

      // add to the histogram
      itsHisto->addDatum(pixVal);
    }
  }

  // determine the mean value and sigma
  itsHisto->getBackGuess(meanVal, sigmaVal);

  if (itsHasWeight){
    itsWeightHisto->getBackGuess(whtMeanVal, whtSigmaVal);
  }

  return;
}

void BackgroundCell::getBackgroundValuesOld(PIXTYPE& bckVal, PIXTYPE& sigmaVal)
{
  BackgroundHistogram* theHisto=NULL;

  // check if something can be done at all
  if (itsMean<=-BIG && itsSigma<=-BIG)
  {
    bckVal   = itsMean;
    sigmaVal = itsSigma;
    return;
  }
  else
  {
    bckVal   = itsMean;
    sigmaVal = itsSigma;
  }

  // create the histogram and fill it
  theHisto = new BackgroundHistogram(itsMean, itsSigma, itsStatNData);
  //theHisto->fillInData(itsCellData, itsNdata);
  theHisto->getBackGuess(bckVal, sigmaVal);

  // release the memory
  delete theHisto;

  return;
}

void BackgroundCell::getStats(const PIXTYPE* cellData, const size_t& ndata, double& mean, double& sigma, size_t& statNData)
{
  //size_t  npix=0;
  PIXTYPE lcut=0;
  PIXTYPE hcut=0;
  double  pixVal=0;

  // go over the data and do the sum ups
  mean=0.0;
  sigma=0.0;
  for (size_t index=0; index<ndata; index++)
  {
    pixVal =cellData[index];
    if (pixVal>-BIG)
    {
      mean  += pixVal;
      sigma += pixVal*pixVal;
      statNData  +=1;
    }
  }

  // check whether there is enough data to continue
  if ((float)statNData < (float)(ndata*BACK_MINGOODFRAC))
  {
    mean  = -BIG;
    sigma = -BIG;
    return;
  }

  // compute mean and sigma of all data
  if (statNData<=0){
    throw Elements::Exception() << "Can not compute meaningful stats with statNData=" << statNData << "!";
  }
  mean /= (double)statNData;
  sigma = sigma/statNData - mean*mean;
  sigma = sigma > std::numeric_limits<double>::epsilon() ? sqrt(sigma):0.0;

  // define the range for the second round
  lcut = (PIXTYPE)(mean - BACK_FKAPPA*sigma);
  hcut = (PIXTYPE)(mean + BACK_FKAPPA*sigma);

  // go over the data, apply
  // the cuts and do the sum ups
  mean  = 0.0;
  sigma = 0.0;
  statNData  = 0;
  for (size_t index=0; index<ndata; index++)
  {
    pixVal =cellData[index];
    if (pixVal>=lcut && pixVal <= hcut)
    {
      mean  += pixVal;
      sigma += pixVal*pixVal;
      statNData  +=1;
    }
  }

  // compute mean and sigma
  // in the restricted range
  if (statNData<=0){
    throw Elements::Exception() << "Can not compute meaningful stats with statNData=" << statNData << "!";
  }

  mean /= (double)statNData;
  sigma = sigma/statNData - mean*mean;
  sigma = sigma>0.0 ? sqrt(sigma):0.0;

  return;
}

void BackgroundCell::getStatsWeight(const PIXTYPE* cellData, const size_t& ndata, const PIXTYPE* cellWeight, const PIXTYPE weightThresh, double& mean, double& sigma, size_t& statNData, double& weightMean, double& weightSigma, size_t& statNWeight)
{
  double lcut=0;
  double hcut=0;
  double weightLcut=0;
  double weightHcut=0;
  double  pixVal=0;
  double  weightVal=0;
  int weightSigma_zero=0;

  // go over the data and make the sums
  // for mean and sigma
  weightMean = 0.0;
  weightSigma = 0.0;
  mean=0.0;
  sigma=0.0;
  for (size_t index=0; index<ndata; index++)
  {
    pixVal    = static_cast<double>(cellData[index]);
    weightVal = static_cast<double>(cellWeight[index]);
    if (weightVal < static_cast<double>(weightThresh) && pixVal > -BIG)
    {
      mean        += pixVal;
      sigma       += pixVal*pixVal;
      weightMean  += weightVal;
      weightSigma += weightVal*weightVal;
      statNData++;

    }
  }

  // check whether there is enough data to continue
  if (static_cast<float>(statNData) < static_cast<float>(ndata*BACK_MINGOODFRAC))
  {
    mean        = -BIG;
    sigma       = -BIG;
    weightMean  = -BIG;
    weightSigma = -BIG;
    return;
  }

  // compute mean and sigma of all data
  mean /= static_cast<double>(statNData);
  sigma = sigma/static_cast<double>(statNData) - mean*mean;
  sigma = sigma > std::numeric_limits<double>::epsilon() ? sqrt(sigma):0.0;
  weightMean /= static_cast<double>(statNData);
  weightSigma = weightSigma/static_cast<double>(statNData) - weightMean*weightMean;
  weightSigma = weightSigma > std::numeric_limits<double>::epsilon() ? sqrt(weightSigma) : 0.0;
  if (weightSigma==0.0 && statNData>0)
	  weightSigma_zero=1;

  // define the ranges for the second round
  lcut = mean - BACK_FKAPPA*sigma;
  hcut = mean + BACK_FKAPPA*sigma;
  weightLcut = weightMean - BACK_FKAPPA*weightSigma;
  weightHcut = weightMean + BACK_FKAPPA*weightSigma;

  // go over the data and make the sums
  // for mean and sigma
  weightMean = 0.0;
  weightSigma = 0.0;
  mean=0.0;
  sigma=0.0;
  statNData=0;
  statNWeight=0;
  for (size_t index=0; index<ndata; index++)
  {
    pixVal    = static_cast<double>(cellData[index]);
    weightVal = static_cast<double>(cellWeight[index]);
    if (weightVal < weightThresh && pixVal <=hcut && pixVal>=lcut)
    {
      mean  += pixVal;
      sigma += pixVal*pixVal;
      statNData++;

      if ((weightVal<=weightHcut && weightVal>=weightLcut) || weightSigma_zero)
      {
        weightMean  += weightVal;
        weightSigma += weightVal*weightVal;
        statNWeight++;
      }
    }
  }

  // compute mean and sigma of the data
  // within the cuts
  if (statNData<=0){
    throw Elements::Exception() << "Can not compute meaningful data stats with statNData=" << statNData;
 }
  mean /= static_cast<double>(statNData);
  sigma = sigma/statNData - mean*mean;
  sigma = sigma>0.0 ? sqrt(sigma):0.0;

  // compute the mean and sigma of the weights
  // within the cuts
  if (statNWeight<=0){
	throw Elements::Exception() << "Can not compute meaningful weight stats with statNWeight=" << statNWeight;
  }
  weightMean /= static_cast<double>(statNWeight);
  weightSigma = weightSigma/static_cast<double>(statNWeight) - weightMean*weightMean;
  weightSigma = weightSigma>0.0 ? sqrt(weightSigma):0.0;
}
} // end of namespace SourceXtractor

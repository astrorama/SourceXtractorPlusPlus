/*
 * Created on Jan 05, 2015
 * @author: mkuemmel@usm.lmu.de
 *
 * Date:     $Date$
 * Revision: $Revision$
 * Author:   $Author$
 */
#include <cmath>
#include "ElementsKernel/Exception.h"       // for Elements Exception
#include "SEImplementation/Background/BackgroundDefine.h"
//#include "BackgroundModule/UtilFunctions.h"
#include "SEImplementation/Background/BackgroundHistogram.h"
#include "SEImplementation/Background/BackgroundCell.h"

namespace SExtractor {

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
    //logger.info() << itsMean << " | "<< itsSigma << " | "<< itsStatNData << " | "<< itsWeightMean << " | "<< itsWeightSigma << " | "<< itsStatNWeight;
  }
  else{
    getStats(itsCellData, itsNdata, itsMean, itsSigma, itsStatNData);
    itsHasWeight=false;
    itsHisto = new BackgroundHistogram(itsMean, itsSigma, itsStatNData);
  }
  //logger.info() << "Data: "<< itsMean << " Sigma: " << itsSigma ;
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
      //logger.info() << whtVal << " wval <> itsWeightThresh: " << itsWeightThresh;
      if (whtVal<itsWeightThresh){
        // add to the histograms
        itsHisto->addDatum(pixVal);
        itsWeightHisto->addDatum(whtVal);
       // logger.info() << whtVal << " wval <> added " << itsWeightThresh;
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
  //logger.info() << meanVal << " bckvals " << sigmaVal;

  if (itsHasWeight){
    itsWeightHisto->getBackGuess(whtMeanVal, whtSigmaVal);
    //logger.info() << whtMeanVal << " whtvals " << whtSigmaVal;
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
  if (theHisto)
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
    //Utils::throwElementsException(std::string("Can not compute meaningful stats with statNData=")+tostr(statNData)+std::string("!"));
    throw Elements::Exception() << "Can not compute meaningful stats with statNData=" << statNData << "!";
  }
  mean /= (double)statNData;
  sigma = sigma/statNData - mean*mean;
  sigma = sigma>0.0 ? sqrt(sigma):0.0;

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
  mean /= (double)statNData;
  sigma = sigma/statNData - mean*mean;
  sigma = sigma>0.0 ? sqrt(sigma):0.0;

  return;
}

void BackgroundCell::getStatsWeight(const PIXTYPE* cellData, const size_t& ndata, const PIXTYPE* cellWeight, const PIXTYPE weightThresh, double& mean, double& sigma, size_t& statNData, double& weightMean, double& weightSigma, size_t& statNWeight)
{
  //size_t  npix=0;
  //size_t  weightNpix=0;
  PIXTYPE lcut=0;
  PIXTYPE hcut=0;
  PIXTYPE weightLcut=0;
  PIXTYPE weightHcut=0;
  double  pixVal=0;
  double  weightVal=0;

  // go over the data and make the sums
  // for mean and sigma
  weightMean = 0.0;
  weightSigma = 0.0;
  mean=0.0;
  sigma=0.0;
  for (size_t index=0; index<ndata; index++)
  {
    pixVal    = cellData[index];
    weightVal = cellWeight[index];
    if (weightVal < weightThresh && pixVal > -BIG)
    {
      mean        += pixVal;
      sigma       += pixVal*pixVal;
      weightMean  += weightVal;
      weightSigma += weightVal*weightVal;
      statNData++;

    }
  }

  // check whether there is enough data to continue
  if ((float)statNData < (float)(ndata*BACK_MINGOODFRAC))
  {
    mean        = -BIG;
    sigma       = -BIG;
    weightMean  = -BIG;
    weightSigma = -BIG;
    return;
  }

  // compute mean and sigma of all data
  mean /= (double)statNData;
  sigma = sigma/statNData - mean*mean;
  sigma = sigma>0.0 ? sqrt(sigma):0.0;
  weightMean /= (double)statNData;
  weightSigma = weightSigma/statNData - weightMean*weightMean;
  weightSigma = weightSigma>0.0 ? sqrt(weightSigma):0.0;

  // define the ranges for the second round
  lcut = (PIXTYPE)(mean - BACK_FKAPPA*sigma);
  hcut = (PIXTYPE)(mean + BACK_FKAPPA*sigma);
  weightLcut = (PIXTYPE)(weightMean - BACK_FKAPPA*weightSigma);
  weightHcut = (PIXTYPE)(weightMean + BACK_FKAPPA*weightSigma);


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
    pixVal    = cellData[index];
    weightVal = cellWeight[index];
    if (weightVal < weightThresh && pixVal <=hcut && pixVal>=lcut)
    {
      mean  += pixVal;
      sigma += pixVal*pixVal;
      statNData++;
      if (weightVal<=weightHcut && weightVal>= weightLcut)
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
    //Utils::throwElementsException(std::string("Can not compute meaningful stats with statNData=")+tostr(statNData)+std::string("!"));
    throw Elements::Exception() << "Can not compute meaningful stats with statNData=" << statNData << "!";
 }
  mean /= (double)statNData;
  sigma = sigma/statNData - mean*mean;
  sigma = sigma>0.0 ? sqrt(sigma):0.0;

  //logger.info() << weightMean << " weightMean <> weightSigma: " << weightSigma << " statNWeight" << statNWeight;

  // compute the mean and sigma of the weights
  // within the cuts
  if (statNWeight<=0){
    //Utils::throwElementsException(std::string("Can not compute meaningful stats with statNData=")+tostr(statNWeight)+std::string("!"));
    throw Elements::Exception() << "Can not compute meaningful stats with statNData=" << statNWeight << "!";
    //<< kernel.cols;
  }
  weightMean /= (double)statNWeight;
  weightSigma = weightSigma/statNWeight - weightMean*weightMean;
  weightSigma = weightSigma>0.0 ? sqrt(weightSigma):0.0;
}
} // end of namespace SExtractor

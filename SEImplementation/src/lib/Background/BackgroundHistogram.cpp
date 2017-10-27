/*
 * Created on Jan 05, 2015
 * @author: mkuemmel@usm.lmu.de
 *
 * Date:     $Date$
 * Revision: $Revision$
 * Author:   $Author$
 */
#include <cmath>
#include <cstddef>
#include "SEImplementation/Background/BackgroundDefine.h"
#include "SEImplementation/Background/BackgroundHistogram.h"

BackgroundHistogram::BackgroundHistogram(const double& mean, const double& sigma, const size_t& ndata)
{
  double step;

  // Astromatic, back.c:back_stat(), falls from the sky!
  step = sqrt(2.0/M_PI)*QUANTIF_NSIGMA/QUANTIF_AMIN;

  // set the basic statistic info
  itsMean      = mean;
  itsSigma     = sigma;
  itsStatNData = ndata;

  // find the right number of levels
  itsNLevels = (size_t)(step*itsStatNData+1);
  if (itsNLevels > QUANTIF_NMAXLEVELS)
    itsNLevels = QUANTIF_NMAXLEVELS;

  // set the scale
  if (itsSigma>0.0)
    itsQscale = 2.0*QUANTIF_NSIGMA*(float)itsSigma/(float)itsNLevels;
  else
    itsQscale = 1.0;

  // set the zero level
  itsQzero = (float)mean - QUANTIF_NSIGMA*(float)sigma;

  // compute the constant
  itsCste = 0.499999 - itsQzero/itsQscale;

  // allocate the histogram
  itsHisto = new int[itsNLevels];
  for (size_t index=0; index<itsNLevels; index++)
    itsHisto[index]=0;
}

BackgroundHistogram::~BackgroundHistogram(){
  // delete the histogram
  if (itsHisto)
    delete[] itsHisto;
  itsHisto=NULL;
}
/*
void BackgroundHistogram::fillInData(const PIXTYPE* cellData, const size_t ndata)
{
  int histIndex;
  PIXTYPE pixVal;

  // go over the data
  for (size_t index=0; index<ndata; index++)
  {
    // take the current value
    pixVal =cellData[index];

    // compute the index in the histogram and enhance
    // the bin if possible
    histIndex = (int)(pixVal/(PIXTYPE)itsQscale + (PIXTYPE)itsCste);
    if (histIndex>=0 && histIndex<(int)itsNLevels)
      itsHisto[histIndex]++;
  }
  //logger.info() << "Created the histogram!";
  return;
}
*/
void BackgroundHistogram::addDatum(const PIXTYPE& pixVal)
{
  int histIndex;

  // compute the index in the histogram and enhance
  // the bin if possible
  histIndex = (int)(pixVal/(PIXTYPE)itsQscale + (PIXTYPE)itsCste);
  if (histIndex>=0 && histIndex<(int)itsNLevels)
    itsHisto[histIndex]++;

  return;
}

void BackgroundHistogram::getBackGuessMod(PIXTYPE& bckVal, PIXTYPE& sigmaVal)
{
  int   *histo, *hilow, *hihigh, *histot;
  unsigned long lowsum, highsum, sum;
  double ftemp, mea, sig, sig1, med, dpix;
  int    i, n, lcut,hcut, nlevelsm1, pix;

  int hilowIndex, hihighIndex;
  //int hcutIndex, lcutIndex;
  int hilowVal, hihighVal;
  unsigned long mySum;
  double myMea, mySig, myDpix;
  //int    myI, myN, myLcut,myHcut, myNlevelsm1, myPix;
  int  myPix;


  histo     = itsHisto;
  hcut      = itsNLevels-1;
  lcut      = 0;
  nlevelsm1 = itsNLevels-1;
  lcut = 0;

  sig = 10.0*nlevelsm1;
  sig1 = 1.0;
  mea = itsMean;
  med = itsMean;
  for (n=100; n-- && (sig>=0.1) && (fabs(sig/sig1-1.0)>BACK_EPS);)
  {
    sig1 = sig;
    sum = 0.0;
    mea = 0.0;
    sig = 0.0;
    lowsum  = 0;
    highsum = 0;

    mySum=0.0;
    myMea=0.0;
    mySig=0.0;

    histot = histo+lcut;
    hilow  = histo+lcut;
    hihigh = histo+hcut;

    // save the values and not the pointers
    //histotIndex=lcut;
    hilowIndex=lcut;
    hihighIndex=hcut;
    //histotVal = histo[histotIndex];
    hilowVal  = histo[hilowIndex];
    hihighVal = histo[hihighIndex];

    for (i=lcut; i<=hcut; i++)
    {
      if (lowsum<highsum)
      {
        lowsum   += *(hilow++);
        hilowIndex++;
        hilowVal = histo[hilowIndex];
      }
      else
      {
        highsum +=  *(hihigh--);
        hihighIndex--;
        hihighVal = histo[hihighIndex];
      }

      sum += (pix = *(histot++));
      mea += (dpix = (double)pix*i);
      sig += dpix*i;

      myPix  = histo[i];
      mySum += myPix;
      myDpix = (double)myPix*i;
      myMea += myDpix;
      mySig += myDpix*i;
    }


    med = hihigh>=histo?((hihigh-histo)+0.5+((double)highsum-lowsum)/(2.0*(*hilow>*hihigh?*hilow:*hihigh))): 0.0;
    /*if (hihigh>=histo){
      printf("%d %d %i %i\n", *hihigh, *hilow, hihighIndex, hihighVal);
      int iTmp;
      if (*hilow>*hihigh)
        iTmp = *hilow;
      else
        iTmp = *hihigh;
      med = (hihigh-histo)+0.5+((double)highsum-lowsum)/(2.0*iTmp);
    }
    else {
      med=0.0;
    }
     */
    if (sum)
    {
      mea /= (double)sum;
      sig = sig/sum - mea*mea;
    }
    sig = sig>0.0?sqrt(sig):0.0;
    lcut = (ftemp=med-3.0*sig)>0.0 ?(int)(ftemp>0.0?ftemp+0.5:ftemp-0.5):0;
    hcut = (ftemp=med+3.0*sig)<nlevelsm1 ?(int)(ftemp>0.0?ftemp+0.5:ftemp-0.5): nlevelsm1;
    //printf("cuts: %i <-> %i\n", lcut, hcut);
  }


  bckVal = fabs(sig)>0.0? (fabs(itsSigma/(sig*itsQscale)-1) < 0.0 ?
      itsQzero+mea*itsQscale
      :(fabs((mea-med)/sig)< 0.3 ?
          itsQzero+(2.5*med-1.5*mea)*itsQscale
          :itsQzero+med*itsQscale))
      :itsQzero+mea*itsQscale;
  sigmaVal = sig*itsQscale;

  return;
}

void BackgroundHistogram::getBackGuess(PIXTYPE& bckVal, PIXTYPE& sigmaVal)
{
  // code taken from Astromatic back.c:back_guess();
  // small adjustments for the class environment;
  // TODO: re-factor that such that it is understandable!!
  int   *histo, *hilow, *hihigh, *histot;
  unsigned long lowsum, highsum, sum;
  double ftemp, mea, sig, sig1, med, dpix;
  int    i, n, lcut,hcut, nlevelsm1, pix;


  histo     = itsHisto;
  hcut      = itsNLevels-1;
  nlevelsm1 = itsNLevels-1;
  lcut = 0;

  sig  = 10.0*nlevelsm1;
  sig1 = 1.0;
  mea  = itsMean;
  med  = itsMean;

  // iterates the clipping until one of the following conditions is fullfilled:
  //  -  sig < 0.1 (meaning that the histogram of the pixels sharply peaks to the mean i.e. all the pixels are in the same histogram bin)
  //  -  fabs(sig/sig1-1) <= 1e-4 (meaning that the difference with respect to the previous iteration is small)

  for (n=100; n-- && (sig>=0.1) && (fabs(sig/sig1-1.0)>BACK_EPS);)
  {
    sig1 = sig;
    sum = mea = sig = 0.0;
    lowsum = highsum = 0;
    histot = hilow = histo+lcut;
    hihigh = histo+hcut;
    for (i=lcut; i<=hcut; i++)       
    {
      if (lowsum<highsum)
        lowsum += *(hilow++);
      else
        highsum +=  *(hihigh--);
      sum += (pix = *(histot++));
      mea += (dpix = (double)pix*i);
      sig += dpix*i;
    }

    med = hihigh>=histo?
        ((hihigh-histo)+0.5+((double)highsum-lowsum)/(2.0*(*hilow>*hihigh?
            *hilow:*hihigh)))
            : 0.0;
    if (sum)
    {
      mea /= (double)sum;
      sig = sig/sum - mea*mea;
    }
    sig = sig>0.0?sqrt(sig):0.0;
    lcut = (ftemp=med-3.0*sig)>0.0 ?(int)(ftemp>0.0?ftemp+0.5:ftemp-0.5):0;
    hcut = (ftemp=med+3.0*sig)<nlevelsm1 ?(int)(ftemp>0.0?ftemp+0.5:ftemp-0.5)
        : nlevelsm1;
  }
  // The following lines can be translated into:
  //
  // if (fabs(sig>0.0))                                   <=== "If sigma is not zero"   
  //   {
  //     if (fabs(itsSigma/(sig*itsQscale)-1) < 0.0)      <=== I think this condition is never satisfied
  // 	{
  // 	  bckVal=itsQzero+mea*itsQscale;
  // 	}
  //     else
  // 	if (fabs((mea-med)/sig) < 0.3)                    <=== This is a measure of how many outliers are in the distribution. 
  // 	  {                                                    If mean and median do not differ too much, we are in the NOT crowded case
  // 	    bckVal=itsQzero+(2.5*med-1.5*mea)*itsQscale;
  // 	  }
  // 	else                                              <=== This is the crowded case
  // 	  {
  // 	    bckVal=itsQzero+med*itsQscale;
  // 	  }	
  //   }
  // else  bckVal=itsQzero+mea*itsQscale;                 <=== "If sigma is zero"
  //
  bckVal = fabs(sig)>0.0? (fabs(itsSigma/(sig*itsQscale)-1) < 0.0 ?
      itsQzero+mea*itsQscale
      :(fabs((mea-med)/sig)< 0.3 ?
          itsQzero+(2.5*med-1.5*mea)*itsQscale
          :itsQzero+med*itsQscale))
      :itsQzero+mea*itsQscale;

  // In the following lines I implement the previous conditions as they are descripted 
  // in the sextractor user's manual (E. Bertin, SExtractor v2.13, pag. 16)
  // 
  // if (fabs(sig>0.0))                                   <=== "If sigma is not zero"   
  //   {
  // 	if (fabs(sig/sig1-1.) < 0.2)                       <=== This is a measure of how many outliers are in the distribution. 
  // 	  {                                                    If mean and median do not differ too much, we are in the NOT crowded case
  // 	    bckVal=itsQzero+mea*itsQscale;
  // 	  }
  // 	else                                              <=== This is the crowded case
  // 	  {
  // 	    bckVal=itsQzero+(2.5*med-1.5*mea)*itsQscale;
  // 	  }	
  //   }
  // else  bckVal=itsQzero+mea*itsQscale;                 <=== "If sigma is zero"
  //


  sigmaVal = sig*itsQscale;

  return;
}


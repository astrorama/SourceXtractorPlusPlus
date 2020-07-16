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
 * Date:     $Date$
 * Revision: $Revision$
 * Author:   $Author$
 */
#include <iostream>
#include  <cstdlib>

#include "fitsio.h"

#include "ElementsKernel/Exception.h"
#include "SEImplementation/Background/Utils.h"
#include "SEImplementation/Background/SE2/BackgroundDefine.h"
#include "SEImplementation/Background/SE2/SE2BackgroundUtils.h"
#include "SEImplementation/Background/SE2/TypedSplineModelWrapper.h"
#include "SEImplementation/Background/SE2/BackgroundCell.h"
#include "SEImplementation/Background/SE2/SE2BackgroundModeller.h"
#define SIZETSUB(X, Y)  ((X) > (Y) ? (X-Y) : (Y-X))
using namespace std;

namespace SourceXtractor {

SE2BackgroundModeller::SE2BackgroundModeller(std::shared_ptr<DetectionImage> image, std::shared_ptr<WeightImage> variance_map, std::shared_ptr<Image<unsigned char>> mask, const unsigned char mask_type_flag)
{
  itsImage          = image;
  itsVariance       = variance_map;
  itsMask           = mask;
  itsMaskType       = mask_type_flag;
  //itsWeightTypeFlag = weight_type_flag;

  //check for variance
  if (variance_map!=nullptr)
    itsHasVariance=true;
  else
    itsHasVariance=false;

  //check for mask
  if (mask!=nullptr)
    itsHasMask=true;
  else
    itsHasMask=false;

  // store the image size
  itsNaxes[0] = (size_t)image->getWidth();
  itsNaxes[1] = (size_t)image->getHeight();
}

SE2BackgroundModeller::~SE2BackgroundModeller() {
  if (itsWhtMeanVals)
    delete[] itsWhtMeanVals;
}

void SE2BackgroundModeller::createSE2Models(std::shared_ptr<TypedSplineModelWrapper<SeFloat>> &bckPtr, std::shared_ptr<TypedSplineModelWrapper<SeFloat>> &varPtr, PIXTYPE &sigFac, const size_t *bckCellSize, const WeightImage::PixelType varianceThreshold, const size_t *filterBoxSize, const float &filterThreshold)
{
  size_t gridSize[2] = {0,0};
  size_t nGridPoints=0;

  long increment[2]={1,1};
  long fpixel[2];
  long lpixel[2];

  size_t nElements=0;
  size_t nData=0;
  size_t subImgNaxes[2] = {0,0};

  PIXTYPE* gridData=NULL;
  PIXTYPE* weightData=NULL;

  //PIXTYPE  undefNumber=-BIG;

  BackgroundCell* oneCell=NULL;

  PIXTYPE* bckMeanVals=NULL;
  PIXTYPE* bckSigVals=NULL;
  PIXTYPE* whtSigVals=NULL;

  size_t gridIndex=0;

  ldiv_t divResult;

  PIXTYPE weightVarThreshold=(PIXTYPE)varianceThreshold;

  // not necessary, since all is in variance
  // re-scale the weight threshold
  //if (itsHasVariance){
  //  rescaleThreshold(weightVarThreshold, weightThreshold);
  //}

  // get the number of grid cells in x
  divResult = std::div(long(itsNaxes[0]), long(bckCellSize[0]));
  gridSize[0] = size_t(divResult.quot);
  if (divResult.rem)
    gridSize[0] += 1;

  // get the number of grid cells in y
  divResult = std::div(long(itsNaxes[1]), long(bckCellSize[1]));
  gridSize[1] = size_t(divResult.quot);
  if (divResult.rem)
    gridSize[1] += 1;

  // compute the number of grid points
  nGridPoints = gridSize[0]*gridSize[1];

  // create the arrays
  bckMeanVals = new PIXTYPE[nGridPoints];
  bckSigVals  = new PIXTYPE[nGridPoints];
  if (itsHasVariance){
    itsWhtMeanVals = new PIXTYPE[nGridPoints];
    whtSigVals  = new PIXTYPE[nGridPoints];
  }

  // give some feedback on the parameters
  bck_model_logger.debug() << "\tBackground cell size=("<<bckCellSize[0]<<"," << bckCellSize[1]<< ")";
  bck_model_logger.debug() << "\tFilter box size=("<<filterBoxSize[0]<<"," << filterBoxSize[1]<< ")";
  bck_model_logger.debug() << "\tThe bad pixel threshold is: "<< weightVarThreshold;

  // iterate over cells in y
  gridIndex=0;
  for (size_t yIndex=0; yIndex<gridSize[1]; yIndex++){

    // set the boundaries in y
    fpixel[1] = (long)yIndex*bckCellSize[1];
    lpixel[1] = yIndex < gridSize[1]-1 ? (long)(yIndex+1)*bckCellSize[1] : (long)itsNaxes[1];

    // iterate over cells in x
    for (size_t xIndex=0; xIndex<gridSize[0]; xIndex++){

      // set the boundaries in x
      fpixel[0] = (long)xIndex*bckCellSize[0];
      lpixel[0] = xIndex < gridSize[0]-1 ? (long)(xIndex+1)*bckCellSize[0] : (long)itsNaxes[0];

      // compute the length of the cell sub-image
      subImgNaxes[0] =(size_t)(lpixel[0]-fpixel[0]);
      subImgNaxes[1] =(size_t)(lpixel[1]-fpixel[1]);

      // some feedback on the corners of the image to be treated
      bck_model_logger.debug() << "Background cell from fpixel=(" << fpixel[0] << "," << fpixel[1] << ") to lpixel=("<< lpixel[0] << "," << lpixel[1] << ")";

      // compute the increments to perhaps limit the number
      // of pixels read in, the total number of elements
      // and the numbers read in x and y
      getMinIncr(nElements, increment, subImgNaxes);

      // define or re-define the buffers
      if (nElements!=nData) {
        delete [] gridData;
        gridData = new PIXTYPE[nElements];
        //if (itsInputWeight){
        if (itsHasVariance){
          delete [] weightData;
          weightData = new PIXTYPE[nElements];
        }
        nData=nElements;
      }

      // load in the image data
      long pixIndex=0;
      for (auto yIndex=fpixel[1]; yIndex<lpixel[1]; yIndex+=increment[1])
        for (auto xIndex=fpixel[0]; xIndex<lpixel[0]; xIndex+=increment[0]){
        	gridData[pixIndex++] = (PIXTYPE)itsImage->getValue(int(xIndex), int(yIndex));
        }
      if (itsHasVariance){
        long pixIndex=0;
        for (auto yIndex=fpixel[1]; yIndex<lpixel[1]; yIndex+=increment[1])
          for (auto xIndex=fpixel[0]; xIndex<lpixel[0]; xIndex+=increment[0])
            weightData[pixIndex++] = (PIXTYPE)itsVariance->getValue(int(xIndex), int(yIndex));
      }
      if (itsHasMask){
        // load in the image data
        long pixIndex=0;
        for (auto yIndex=fpixel[1]; yIndex<lpixel[1]; yIndex+=increment[1])
          for (auto xIndex=fpixel[0]; xIndex<lpixel[0]; xIndex+=increment[0], pixIndex++)
            //if (!itsMask->getValue(int(xIndex), int(yIndex)))
              if (itsMask->getValue(int(xIndex), int(yIndex)) & itsMaskType){
                gridData[pixIndex] = -BIG;
                bck_model_logger.debug() << "\tReplacing data value";
              }
      }

      // create a background cell compute and store the values
      // and then delete the cell again
      oneCell = new BackgroundCell(gridData, nElements, weightData, weightVarThreshold);
      if (itsHasVariance)
        oneCell->getBackgroundValues(bckMeanVals[gridIndex], bckSigVals[gridIndex], itsWhtMeanVals[gridIndex], whtSigVals[gridIndex]);
      else
        oneCell->getBackgroundValues(bckMeanVals[gridIndex], bckSigVals[gridIndex]);
      delete oneCell;

      // enhance the grid index
      gridIndex++;
    }
  }

  // do some filtering on the data
  filter(bckMeanVals, bckSigVals, gridSize, filterBoxSize, filterThreshold);
  if (itsHasVariance){
    filter(itsWhtMeanVals, whtSigVals, gridSize, filterBoxSize, filterThreshold);
  }

  if (itsHasVariance){
  //if (itsHasVariance && (itsWeightTypeFlag & (VAR_FIELD|WEIGHT_FIELD))){
    // compute the scaling factor
    computeScalingFactor(itsWhtMeanVals, bckSigVals, sigFac, nGridPoints);
  }
  else{
    sigFac=0.0;
  }

  // convert the grid of rms values to variance
  for (size_t index=0; index<nGridPoints; index++)
    bckSigVals[index] *= bckSigVals[index];

  // create the splined interpolation images for background and variance
  bckPtr = TypedSplineModelWrapper<SeFloat>::create(itsNaxes, bckCellSize, gridSize, bckMeanVals);
  varPtr = TypedSplineModelWrapper<SeFloat>::create(itsNaxes, bckCellSize, gridSize, bckSigVals);

   // release memory
  delete [] whtSigVals;
  delete [] gridData;
  delete [] weightData;
}

void SE2BackgroundModeller::getMinIncr(size_t &nElements, long* incr, const size_t * subImgNaxes)
{
  float axisRatio;
  ldiv_t divResult;

  // copy the original dimensions
  size_t tmpImgNaxes[2] = {subImgNaxes[0], subImgNaxes[1]};

  // set the default increment
  incr[0] = 1;
  incr[1] = 1;

  // compute the number of elements;
  // check if something needs to be done at all
  nElements = tmpImgNaxes[0]*tmpImgNaxes[1];
  if (nElements <= BACK_BUFSIZE)
	  return;

  // compute the axis ratio
  axisRatio = float(tmpImgNaxes[0]) / float(tmpImgNaxes[1]);

  // iterate until the number is small enough
  while (nElements > BACK_BUFSIZE)
  {
    // change the increments such
    // that pixels sampled in x and y
    // are about equal
    if (axisRatio >= 1.0)
      incr[0] += 1;
    else
      incr[1] += 1;

    // get the number of pixels sampled in x
    divResult = std::div(long(subImgNaxes[0]), long(incr[0]));
    tmpImgNaxes[0] = size_t(divResult.quot);
    if (divResult.rem)
    	tmpImgNaxes[0] += 1;

    // get the number of pixels sampled in y
    divResult = std::div(long(subImgNaxes[1]), long(incr[1]));
    tmpImgNaxes[1] = size_t(divResult.quot);
    if (divResult.rem)
    	tmpImgNaxes[1] += 1;

    // re-compute the number of elements and the
    // axis ratio
    nElements = tmpImgNaxes[0]*tmpImgNaxes[1];
    axisRatio = float(tmpImgNaxes[0]) / float(tmpImgNaxes[1]);
  }

  // some feedback for the new increments and sizes
  bck_model_logger.debug() << "New increment=(" << incr[0] << "," << incr[1] << ") sampledPixels=("<< tmpImgNaxes[0] << "," << tmpImgNaxes[1] << ") nElements=" << nElements;

  return;
}

void SE2BackgroundModeller::filter(PIXTYPE* bckVals, PIXTYPE* sigmaVals,const size_t* gridSize, const size_t* filterSize, const float &filterThreshold)
{
  // replace undefined values
  replaceUNDEF(bckVals,  sigmaVals, gridSize);

  // do a median filtering of the values
  filterMedian(bckVals, sigmaVals, gridSize, filterSize, filterThreshold);

  return;
}

void SE2BackgroundModeller::replaceUNDEF(PIXTYPE* bckVals, PIXTYPE* sigmaVals,const size_t* gridSize)
{
  PIXTYPE  *back=NULL;
  PIXTYPE  *sigma=NULL;
  PIXTYPE  *backMod=NULL;
  PIXTYPE   val;
  PIXTYPE   sval;
  float     dist=0.;
  float     distMin=0;
  size_t    iAct,jAct, nx,ny, nmin;
  size_t    np;
  bool      hasHoles=false;

  // take the sizing information
  nx = gridSize[0];
  ny = gridSize[1];
  np = gridSize[0]*gridSize[1];

  // check whether something needs to be done
  for (size_t index=0; index< np; index++)
  {
    if (bckVals[index]<=-BIG)
    {
      hasHoles=true;
      break;
    }
  }

  // nothing to do,
  // just go back
  if (!hasHoles)
    return;

  // give some feedback that undefined data is replaced
  bck_model_logger.debug() << "\tReplacing undefined data";

  // vector for the final
  // background values
  backMod = new PIXTYPE[np];

  // go to the array start
  back  = bckVals;
  sigma = sigmaVals;

  // look for `bad' meshes and interpolate them if necessary
  val  = 0.0;
  sval = 0.0;
  iAct=0;
  for (size_t py=0; py<ny; py++)
  {
    for (size_t px=0; px<nx; px++)
    {
      // compute the current index
      iAct = py*nx+px;

      // transfer the value
      backMod[iAct]=back[iAct];

      // check for undefined data
      if (back[iAct]<=-BIG)
      {

        // seek the closest data points,
        // search over the  entire array
        distMin = BIG;
        nmin    = 0;
        for (size_t y=0; y<ny; y++)
        {
          for (size_t x=0; x<nx; x++)
          {
            // compute the current index
            jAct = y*nx+x;

            if (back[jAct]>-BIG)
            {
              // compute the pixel distance
              dist = (float)SIZETSUB(x,px)*SIZETSUB(x,px)+SIZETSUB(y,py)*SIZETSUB(y,py);

              // check for a new minimum distance
              if (dist<distMin)
              {
                // start new average values
                val = back[jAct];
                sval = sigma[jAct];
                nmin = 1;
                distMin = dist;
              }
              else if (fabs(dist-distMin)<1e-05)
              {
                // add equal distance pixel for averaging
                val += back[jAct];
                sval += sigma[jAct];
                nmin++;
              }
            }
          }
        }
        // take the mean of the closest
        // defined data points
        backMod[iAct] = nmin ? val/nmin: 0.0;
        sigma[iAct]   = nmin ? sval/nmin: 1.0;
      }
    }
  }

  // push the modified values back
  for (size_t index=0; index< np; index++)
  {
    bckVals[index] =backMod[index];
  }

  // release the memory
  if (backMod)
    delete [] backMod;

  return;
}

void SE2BackgroundModeller::filterMedian(PIXTYPE* bckVals, PIXTYPE* sigmaVals, const size_t* gridSize, const size_t* filterSize, const float filterThresh)
{
  PIXTYPE  *back, *sigma, *sigmat;
  PIXTYPE* backFilt=NULL;
  PIXTYPE* sigmaFilt=NULL;
  PIXTYPE* bmask=NULL;
  PIXTYPE* smask=NULL;
  PIXTYPE allSigmaMed, median; //allBckMed
  int    i,nx,ny,npx,npx2,npy,npy2,x,y;
  int np;

  // check whether something needs to be done at all
  // note that the code would run nevertheless
  if (filterSize[0]<2 && filterSize[1]<2)
    return;

  // Note: I am converting the "size_t" to int's since
  //       there are computations done down.

  // take the sizing information
  nx = (int)gridSize[0];
  ny = (int)gridSize[1];
  np = nx*ny;

  // store the filter size
  npx = (int)filterSize[0]/2;
  npy = (int)filterSize[1]/2;
  npy *= nx;

  // allocate space for the work area
  bmask  = new PIXTYPE[(2*npx+1)*(2*npy+1)];
  smask  = new PIXTYPE[(2*npx+1)*(2*npy+1)];

  // allocate space for filtered arrays
  backFilt  = new PIXTYPE[np];
  sigmaFilt = new PIXTYPE[np];

  // store the arrays locally
  back  = bckVals;
  sigma = sigmaVals;

  // go over all y
  for (int py=0; py<np; py+=nx)
  {
    // limit the filter box in y
    npy2 = np - py - nx;
    if (npy2>npy)
      npy2 = npy;
    if (npy2>py)
      npy2 = py;

    // go over all x
    for (int px=0; px<nx; px++)
    {
      // limit the filter box in x
      npx2 = nx - px - 1;
      if (npx2>npx)
        npx2 = npx;
      if (npx2>px)
        npx2 = px;

      // store all values in the box
      // in an array
      i=0;
      for (int dpy = -npy2; dpy<=npy2; dpy+=nx)
      {
        y = py+dpy;
        for (int dpx = -npx2; dpx <= npx2; dpx++)
        {
          x = px+dpx;
          bmask[i] = back[x+y];
          smask[i++] = sigma[x+y];
        }
      }

      // compute the median, check
      // whether the median is above the threshold
      median = SE2BackgroundUtils::fqMedian(bmask, i);
      if (fabs((median-back[px+py]))>=(PIXTYPE)filterThresh)
      {
        // use the median values
        backFilt[px+py] = median;
        sigmaFilt[px+py] = SE2BackgroundUtils::fqMedian(smask, i);
      }
      else
      {
        // use the original value
        backFilt[px+py] = back[px+py];
        sigmaFilt[px+py] = sigma[px+py];
      }
    }
  }

  // transfer the filtered background values back
  for (int index=0; index<np; index++)
    back[index] = backFilt[index];

  // transfer the filtered sigma values back
  for (int index=0; index<np; index++)
    sigma[index] = sigmaFilt[index];

  // compute the median values for the background
  // and the sigma
  //allBckMed   = SE2BackgroundUtils::fqMedian(backFilt, np);
  allSigmaMed = SE2BackgroundUtils::fqMedian(sigmaFilt, np);

  // NOTE: I don't understand what that does.
  //       Why should the median sigma be <.0?
  if (allSigmaMed<=0.0)
  {
    sigmat = sigmaFilt+np;
    for (i=np; i-- && *(--sigmat)>0.0;);
    if (i>=0 && i<(np-1))
      allSigmaMed = SE2BackgroundUtils::fqMedian(sigmat+1, np-1-i);
    else
    {
      //if (field->flags&(DETECT_FIELD|MEASURE_FIELD))
      //  warning("Image contains mainly constant data; ",
      //      "I'll try to cope with that...");
      //field->backsig = 1.0;
      allSigmaMed = 1.0;
    }
  }

  // release memory
  delete [] sigmaFilt;
  delete [] backFilt;
  delete [] bmask;
  delete [] smask;

  return;
}

void SE2BackgroundModeller::computeScalingFactor(PIXTYPE* whtMeanVals, PIXTYPE* bckSigVals, PIXTYPE& sigFac, const size_t nGridPoints)
{

  size_t nr = 0;
  size_t lowIndex=0;
  PIXTYPE* ratio=NULL;

  PIXTYPE actRatio;
  //PIXTYPE tmp;

  // allocate memory
  ratio = new PIXTYPE[nGridPoints];

  // form the list of ratios between measured sigma values
  // and the sigma's properly derived from the weights

  for (size_t index=0; index<nGridPoints; index++){
    if (whtMeanVals[index]>0.0){
      //actRatio = bckSigVals[index] / sqrt(whtMeanVals[index]);
      actRatio = bckSigVals[index] * bckSigVals[index] / whtMeanVals[index]; // scaling factor for the variance image
      if (actRatio>0.0){
        ratio[nr]=actRatio;
        nr++;
      }
    }
  }

  // use the median ratio as scaling factor
  sigFac = SE2BackgroundUtils::fqMedian(ratio, nr);

  // count leading 0.0 values in the list
  for (lowIndex=0; lowIndex<nr && ratio[lowIndex]<=0.0; lowIndex++);

  // re-calculated the median, omitting
  // the leading 0.0 values; I can't see
  // how there should be leading zeros
  // TODO:  check whether this make sense
  if (lowIndex>0){
    if (lowIndex<nr){
      // make a log message
      bck_model_logger.debug() << "\tRe-calculating the scaling due to leading zeros";
      sigFac = SE2BackgroundUtils::fqMedian(ratio+lowIndex, nr-lowIndex);
    }
    else {
      //warning("Null or negative global weighting factor:","defaulted to 1");
      bck_model_logger.debug() << "\tNull or negative global weighting factor: " << " | " << lowIndex << "defaulted to 1 " << nr;
      sigFac = 1.0;
    }
  }

  delete [] ratio;
  return;
}

void SE2BackgroundModeller::rescaleThreshold(PIXTYPE &weightVarThreshold, const PIXTYPE &weightThreshold)
{
  // the threshold needs to be larger than zero
  if (weightThreshold<0.0){
    throw Elements::Exception() << "The weight threshold is: " << weightThreshold << " but can not be smaller than 0.0";
  }

  // check the type flag
  switch (itsWeightTypeFlag){

  case VAR_FIELD:
    // just copy for variance
    weightVarThreshold = weightThreshold;
    break;

  case RMS_FIELD:
    // square for rms
    weightVarThreshold = weightThreshold*weightThreshold;
    break;

  case WEIGHT_FIELD:
    // give default or invert for weight
    if (weightThreshold>0.0){
      weightVarThreshold = 1.0/weightThreshold;
    }
    else {
      weightVarThreshold = BIG;
    }
    break;

  default:
    // this is the default
    weightVarThreshold = BIG;
    break;
  }
  return;
}

PIXTYPE *SE2BackgroundModeller::getWhtMeanVals()
{
  return itsWhtMeanVals;
}
} // end of namespace SourceXtractor

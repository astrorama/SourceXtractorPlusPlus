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
#include "ElementsKernel/Exception.h"       // for Elements Exception
#include "SEImplementation/Background/BackgroundDefine.h"
#include "SEImplementation/Background/SplineModel.h"
#include "SEImplementation/Background/BackgroundCell.h"
#include "SEImplementation/Background/SE2BackgroundModeller.h"
#define SIZETSUB(X, Y)  ((X) > (Y) ? (X-Y) : (Y-X))
using namespace std;

SE2BackgroundModeller::SE2BackgroundModeller(std::shared_ptr<SExtractor::DetectionImage> image, std::shared_ptr<SExtractor::WeightImage> variance_map, std::shared_ptr<SExtractor::Image<unsigned char>> mask, const int weight_type_flag)
{
  itsImage          = image;
  itsVariance       = variance_map;
  itsMask           = mask;
  itsWeightTypeFlag = weight_type_flag;

  // TODO: needs to be done properly!
  itsHasWeights=false;
  itsHasMask=false;

  itsNaxes[0] = (size_t)image->getWidth();
  itsNaxes[1] = (size_t)image->getHeight();
}

SE2BackgroundModeller::SE2BackgroundModeller(const boost::filesystem::path& fits_filename, const boost::filesystem::path& weight_filename, const boost::filesystem::path& mask_filename, const int mask_type, const int weight_type_flag)
{
  int status=0;
  long naxes[2];

  // copy all input
  itsInputFileName   = fits_filename;
  itsInputWeightName = weight_filename;
  itsInputMaskName = mask_filename;
  itsWeightTypeFlag  = weight_type_flag;
  itsMaskType  = mask_type;

  // open the FITS file and store the pointer; give some logger info
  fits_open_image(&itsInputFits, itsInputFileName.generic_string().c_str(), READONLY, &status);
  if (status){
    throw Elements::Exception() << "Problem opening the image: " << itsInputFileName << "!";
    //Utils::throwElementsException(std::string("Problem opening the image: ")+itsInputFileName.generic_string()+std::string("!"));
  }

  // open the mask file and store the pointer; give some logger info
  if (itsInputMaskName.generic_string().size()>0){
    fits_open_image(&itsInputMask, itsInputMaskName.generic_string().c_str(), READONLY, &status);
    if (status){
      throw Elements::Exception() << "Problem opening the mask image: " << itsInputMaskName << "!";
      //Utils::throwElementsException(std::string("Problem opening the mask image: ")+itsInputMaskName.generic_string()+std::string("!"));
    }
    itsHasMask=true;
  }
  // open the weight file and store the pointer; give some logger info
  if (itsInputWeightName.generic_string().size()>0){
    // determine the appropriate mode
    //int fitsReadMode=READONLY;
   // if (itsWeightTypeFlag & (VAR_FIELD|WEIGHT_FIELD)){
    //  fitsReadMode=READWRITE;
    //}

    // open the FITS file and store the pointer; give some logger info
    //fits_open_image(&itsInputWeight, itsInputWeightName.generic_string().c_str(), fitsReadMode, &status);
    fits_open_image(&itsInputWeight, itsInputWeightName.generic_string().c_str(), READONLY, &status);
    if (status){
      //char aString[81];
      //FILE *wFile;
     // wFile = fopen(itsInputWeightName.generic_string().c_str(), "r");
     // if (wFile==NULL)
     //   Utils::throwElementsException(std::string("Problem opening the weight image!!!"));
      //fgets(aString, 80, wFile);
      //Utils::generalLogger(std::string(aString));
     // fclose(wFile);
 
      //char error[80];
      //fits_get_errstatus(status, error);
      //Utils::throwElementsException(std::string("Problem opening the weight image: ")+itsInputWeightName.generic_string()+std::string(error)+std::string("!")+tostr(status));
      //throw Elements::Exception() << "Problem opening the weight image: " << itsInputWeightName << "!";
      throw Elements::Exception() << "Problem opening the weight image: " << itsInputWeightName << "!";
      //Utils::throwElementsException(std::string("Problem opening the weight image: ")+itsInputWeightName.generic_string()+std::string("!"));
    }
    itsHasWeights=true;
  }

  // get the basic image information;
  // make sure the image has two dimensions
  fits_get_img_param(itsInputFits, 2,  &itsBitpix, &itsNaxis, naxes, &status);
  if (status){
    throw Elements::Exception() << "Problem reading parameters from image: " << itsInputFileName << "!";
    //Utils::throwElementsException(std::string("Problem reading parameters from image: ")+itsInputFileName.generic_string()+std::string("!"));
  }
  if (itsNaxis!=2){
    throw Elements::Exception() << "The image: " << itsInputFileName << " has " << itsNaxis <<"!=2 dimensions!";
    //Utils::throwElementsException(std::string("The image: ")+itsInputFileName.generic_string()+std::string(" has ")+tostr(itsNaxis)+std::string("!=2 dimensions!)"));
  }
  itsNaxes[0] = (size_t)naxes[0];
  itsNaxes[1] = (size_t)naxes[1];

  // make sure the dimensions of the
  // weight file agree with the data image
  if (itsInputWeight){
    checkCompatibility(itsInputWeight, itsNaxes);
  }
  if (itsInputMask){
    checkCompatibility(itsInputMaskName, itsInputMask, itsNaxes);
  }
}

SE2BackgroundModeller::~SE2BackgroundModeller(){
  int status=0;

  // close the FITS file
  if (itsInputFits){
    fits_close_file(itsInputFits, &status);
    if (status){
      throw Elements::Exception() << "Problem closing the image: " << itsInputFileName << "!";
      //Utils::throwElementsException(std::string("Problem closing the image: ")+itsInputFileName.generic_string()+std::string("!"));
    }
  }
  itsInputFits=NULL;

  // close the weight file
  if (itsInputWeight){
    fits_close_file(itsInputWeight, &status);
    if (status){
      char fitsErrorChar[1028];
      fits_get_errstatus(status, fitsErrorChar);
      //Utils::throwElementsException(std::string(fitsErrorChar));
      throw Elements::Exception() << "Problem closing the image: " << itsInputWeightName << "!";
      //Utils::throwElementsException(std::string("Problem closing the image: ")+itsInputWeightName.generic_string()+std::string("!"));
    }
  }
  itsInputWeight=NULL;

  //close the mask file
  if (itsInputMask){
    fits_close_file(itsInputMask, &status);
    if (status){
      char fitsErrorChar[1028];
      fits_get_errstatus(status, fitsErrorChar);
      //Utils::throwElementsException(std::string(fitsErrorChar));
      throw Elements::Exception() << "Problem closing the image: " << itsInputMaskName << "!";
      //Utils::throwElementsException(std::string("Problem closing the image: ")+itsInputMaskName.generic_string()+std::string("!"));
    }
  }
  itsInputMask=NULL;
  //
  if (itsWhtMeanVals)
    delete [] itsWhtMeanVals;
  //
}

void SE2BackgroundModeller::createSE2Models(SplineModel **bckSpline, SplineModel **sigmaSpline, PIXTYPE &sigFac, const size_t *bckCellSize, const PIXTYPE weightThreshold, const bool &storeScaleFactor)
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
  int* maskData=NULL;

  PIXTYPE  undefNumber=-BIG;

  BackgroundCell* oneCell=NULL;

  PIXTYPE* bckMeanVals=NULL;
  PIXTYPE* bckSigVals=NULL;
  PIXTYPE* whtSigVals=NULL;

  size_t gridIndex=0;

  ldiv_t divResult;

  PIXTYPE weightVarThreshold=0.0;

  // re-scale the weight threshold
  if (itsHasWeights){
    rescaleThreshold(weightVarThreshold, weightThreshold);
  }

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
  if (itsHasWeights){
    itsWhtMeanVals = new PIXTYPE[nGridPoints];
    whtSigVals  = new PIXTYPE[nGridPoints];
  }

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

      // compute the increments to limit the number
      // of pixels read in, the total number of elements
      // and the numbers read in x and y
      getMinIncr(nElements, increment, subImgNaxes);

      // define or re-define the buffers
      if (nElements!=nData) {
        delete [] gridData;
        gridData = new PIXTYPE[nElements];
        if (itsInputWeight){
          delete [] weightData;
          weightData = new PIXTYPE[nElements];
        }
        if (itsInputMask){
          delete [] maskData;
          maskData = new int[nElements];
        }
        nData=nElements;
      }

      // load in the image data
      long pixIndex=0;
      for (auto yIndex=fpixel[1]; yIndex<lpixel[1]; yIndex+=increment[1])
        for (auto xIndex=fpixel[0]; xIndex<lpixel[0]; xIndex+=increment[0])
          gridData[pixIndex++] = (PIXTYPE)itsImage->getValue(int(xIndex), int(yIndex));

      // read in the data, complain if there are problems
      /*fits_read_subset(itsInputFits, TFLOAT, fpixel, lpixel, increment, &undefNumber, gridData, &anynul, &status);
      std::cout << "status: " << status  << std::endl;
      if (status){
        throw Elements::Exception() << "Problems reading background cell:" << gridSize[0] << "," << gridSize[1] << " from image: " << itsInputFileName << "!";
        //Utils::throwElementsException(std::string("Problems reading background cell:")+tostr(gridSize[0])+std::string(",")+tostr(gridSize[1])+std::string(" from image: ")+itsInputFileName.generic_string());
      }
      */
      //throw Elements::Exception() << "Stop right now!";
      if (itsHasWeights){
        throw Elements::Exception() << "Not yet implemented!";
        /*
        fits_read_subset(itsInputWeight, TFLOAT, fpixel, lpixel, increment, &undefNumber, weightData, &anynul, &status);
        if (status){
          throw Elements::Exception() << "Problems reading background weight cell:" << gridSize[0] << "," << gridSize[1] << " from image: " << itsInputWeightName << "!";
        }
        // convert the weight to variance
        ///Utils::weightToVar(weightData, nElements, itsWeightTypeFlag);
        throw Elements::Exception() << "It should not come to here!!!! Utils::weightToVar(weightData, nElements, itsWeightTypeFlag)";
        */
      }
      if (itsHasMask){
        throw Elements::Exception() << "Not yet implemented!";
        /*
        fits_read_subset(itsInputMask, TINT, fpixel, lpixel, increment, &undefNumber, maskData, &anynul, &status);
        if (status){
          throw Elements::Exception() << "Problems reading background weight cell:" << gridSize[0] << "," << gridSize[1] << " from image: " << itsInputWeightName << "!";
          //Utils::throwElementsException(std::string("Problems reading background mask cell:")+tostr(gridSize[0])+std::string(",")+tostr(gridSize[1])+std::string(" from image: ")+itsInputMaskName.generic_string());
        }
        else
          // if the pixel belongs to the mask, set it equal to -BIG
          for (size_t idx=0;idx<nElements;idx++)
            if (maskData[idx] & itsMaskType)
              gridData[idx] = -BIG;
              */
      }

      // create a background cell compute and store the values
      // and then delete the cell again
      oneCell = new BackgroundCell(gridData, nElements, weightData, weightVarThreshold);
      if (itsHasWeights)
        oneCell->getBackgroundValues(bckMeanVals[gridIndex], bckSigVals[gridIndex], itsWhtMeanVals[gridIndex], whtSigVals[gridIndex]);
      else
        oneCell->getBackgroundValues(bckMeanVals[gridIndex], bckSigVals[gridIndex]);
      delete oneCell;
      //std::cout <<gridIndex<<":"<<bckMeanVals[gridIndex]<<":"<<bckSigVals[gridIndex]<<" ";
      // enhance the grid index
      gridIndex++;
    }
  }


  if (itsHasWeights && (itsWeightTypeFlag & (VAR_FIELD|WEIGHT_FIELD))){

    // compute the scaling factor
    computeScalingFactor(itsWhtMeanVals, bckSigVals, sigFac, nGridPoints);

    // store the scaling factor in the weight image
    if (storeScaleFactor){
      storeScalingFactor(sigFac);
    }
 }
  else{
    sigFac=0.0;
  }

  // delete the previous spline object
  if (*bckSpline)
    delete *bckSpline;

  // delete the previous spline object
  if (*sigmaSpline)
    delete *sigmaSpline;

  // create the spline objects for sigma and background
  *bckSpline   = new SplineModel(itsNaxes, bckCellSize, gridSize, bckMeanVals);
  *sigmaSpline = new SplineModel(itsNaxes, bckCellSize, gridSize, bckSigVals);

  // release memory
  if (whtSigVals)
    delete [] whtSigVals;
  // release memory
  if (gridData)
    delete [] gridData;
  if (weightData)
    delete [] weightData;
}

void SE2BackgroundModeller::createModels(SplineModel **bckSpline, SplineModel **sigmaSpline, PIXTYPE &sigFac, const size_t *bckCellSize, const PIXTYPE weightThreshold, const bool &storeScaleFactor)
  {
  int status=0;
  int anynul=0;

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
  int* maskData=NULL;

  PIXTYPE  undefNumber=-BIG;

  BackgroundCell* oneCell=NULL;

  PIXTYPE* bckMeanVals=NULL;
  PIXTYPE* bckSigVals=NULL;
  //PIXTYPE* whtMeanVals=NULL;
  PIXTYPE* whtSigVals=NULL;

  size_t gridIndex=0;

  ldiv_t divResult;

  PIXTYPE weightVarThreshold=0.0;

  // re-scale the weight threshold
  if (itsHasWeights){
    rescaleThreshold(weightVarThreshold, weightThreshold);
  }

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
  if (itsHasWeights){
    itsWhtMeanVals = new PIXTYPE[nGridPoints];
    whtSigVals  = new PIXTYPE[nGridPoints];
  }

  // iterate over cells in y
  gridIndex=0;
  for (size_t yIndex=0; yIndex<gridSize[1]; yIndex++){

    // set the boundaries in y
    fpixel[1] = (long)yIndex*bckCellSize[1]+1;
    lpixel[1] = yIndex < gridSize[1]-1 ? (long)(yIndex+1)*bckCellSize[1] : (long)itsNaxes[1];

    // iterate over cells in x
    for (size_t xIndex=0; xIndex<gridSize[0]; xIndex++){

      // set the boundaries in y
      fpixel[0] = (long)xIndex*bckCellSize[0]+1;
      lpixel[0] = xIndex < gridSize[0]-1 ? (long)(xIndex+1)*bckCellSize[0] : (long)itsNaxes[0];

      // compute the length of the cell sub-image
      subImgNaxes[0] =(size_t)(lpixel[0]-fpixel[0]+1);
      subImgNaxes[1] =(size_t)(lpixel[1]-fpixel[1]+1);

      // compute the increments to limit the number
      // of pixels read in, the total number of elements
      // and the numbers read in x and y
      getMinIncr(nElements, increment, subImgNaxes);

      // define or re-define the buffers
      if (nElements!=nData) {
        delete [] gridData;
        gridData = new PIXTYPE[nElements];
        if (itsInputWeight){
          delete [] weightData;
          weightData = new PIXTYPE[nElements];
        }
        if (itsInputMask){
          delete [] maskData;
          maskData = new int[nElements];
        }
        nData=nElements;
      }

      // read in the data, complain if there are problems
      fits_read_subset(itsInputFits, TFLOAT, fpixel, lpixel, increment, &undefNumber, gridData, &anynul, &status);
      if (status){
        throw Elements::Exception() << "Problems reading background cell:" << gridSize[0] << "," << gridSize[1] << " from image: " << itsInputFileName << "!";
        //Utils::throwElementsException(std::string("Problems reading background cell:")+tostr(gridSize[0])+std::string(",")+tostr(gridSize[1])+std::string(" from image: ")+itsInputFileName.generic_string());
      }
      if (itsHasWeights){
        fits_read_subset(itsInputWeight, TFLOAT, fpixel, lpixel, increment, &undefNumber, weightData, &anynul, &status);
        if (status){
          throw Elements::Exception() << "Problems reading background weight cell:" << gridSize[0] << "," << gridSize[1] << " from image: " << itsInputWeightName << "!";
          //Utils::throwElementsException(std::string("Problems reading background weight cell:")+tostr(gridSize[0])+std::string(",")+tostr(gridSize[1])+std::string(" from image: ")+itsInputWeightName.generic_string());
        }
        // convert the weight to variance
        ///Utils::weightToVar(weightData, nElements, itsWeightTypeFlag);
        throw Elements::Exception() << "It should not come to here!!!! Utils::weightToVar(weightData, nElements, itsWeightTypeFlag)";
      }
      if (itsHasMask){
        fits_read_subset(itsInputMask, TINT, fpixel, lpixel, increment, &undefNumber, maskData, &anynul, &status);
        if (status){
          throw Elements::Exception() << "Problems reading background weight cell:" << gridSize[0] << "," << gridSize[1] << " from image: " << itsInputWeightName << "!";
          //Utils::throwElementsException(std::string("Problems reading background mask cell:")+tostr(gridSize[0])+std::string(",")+tostr(gridSize[1])+std::string(" from image: ")+itsInputMaskName.generic_string());
        }
        else
          // if the pixel belongs to the mask, set it equal to -BIG
          for (size_t idx=0;idx<nElements;idx++)
            if (maskData[idx] & itsMaskType)
              gridData[idx] = -BIG;
      }

      // create a background cell compute and store the values
      // and then delete the cell again
      oneCell = new BackgroundCell(gridData, nElements, weightData, weightVarThreshold);
      if (itsHasWeights)
        oneCell->getBackgroundValues(bckMeanVals[gridIndex], bckSigVals[gridIndex], itsWhtMeanVals[gridIndex], whtSigVals[gridIndex]);
      else
        oneCell->getBackgroundValues(bckMeanVals[gridIndex], bckSigVals[gridIndex]);
      delete oneCell;

      // enhance the grid index
      gridIndex++;
    }
  }


  if (itsHasWeights && (itsWeightTypeFlag & (VAR_FIELD|WEIGHT_FIELD))){

    // compute the scaling factor
    computeScalingFactor(itsWhtMeanVals, bckSigVals, sigFac, nGridPoints);

    // store the scaling factor in the weight image
    if (storeScaleFactor){
      storeScalingFactor(sigFac);
    }
 }
  else{
    sigFac=0.0;
  }


  // delete the previous spline object
  if (*bckSpline)
    delete *bckSpline;

  // delete the previous spline object
  if (*sigmaSpline)
    delete *sigmaSpline;

  // create the spline objects for sigma and background
  *bckSpline   = new SplineModel(itsNaxes, bckCellSize, gridSize, bckMeanVals);
  *sigmaSpline = new SplineModel(itsNaxes, bckCellSize, gridSize, bckSigVals);

  // make a log message
  //std::string logMessage = std::string("Median value of sigma spline==")+tostr((*sigmaSpline)->getMedian());
  //Utils::generalLogger(logMessage);

  // release memory
  //if (itsWhtMeanVals)
  //delete [] whtMeanVals;
  if (whtSigVals)
    delete [] whtSigVals;

  // release memory
  if (gridData)
    delete [] gridData;
  if (weightData)
    delete [] weightData;
}

void SE2BackgroundModeller::getMinIncr(size_t &nElements, long* incr, size_t * subImgNaxes)
{
  float axisRatio;
  ldiv_t divResult;

  // compute the number of elements
  nElements = subImgNaxes[0]*subImgNaxes[1];

  // check if something needs to be done at all
  if (nElements <= BACK_BUFSIZE)
  {
    // give defaults
    incr[0]=1;
    incr[1]=1;
    return;
  }

  // compute the axis ratio
  axisRatio = float(subImgNaxes[0]) / float(subImgNaxes[1]);

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
    subImgNaxes[0] = size_t(divResult.quot);
    if (divResult.rem)
      subImgNaxes[0] += 1;

    // get the number of pixels sampled in y
    divResult = std::div(long(subImgNaxes[1]), long(incr[1]));
    subImgNaxes[1] = size_t(divResult.quot);
    if (divResult.rem)
      subImgNaxes[1] += 1;

    // re-compute the number of elements and the
    // axis ratio
    nElements = subImgNaxes[0]*subImgNaxes[1];
    axisRatio = float(subImgNaxes[0]) / float(subImgNaxes[1]);
  }
  return;
  /*
   * NOTE: python snippet with the same functionality
   *
   * def makeIncrementNaxes(nPixMax, naxes):
      import math

      minIncr = [1,1]

      nPixAct = naxes[0]*naxes[1]

      # "natural" increments
      if nPixAct<=nPixMax:
          return minIncr

      # the factor of too many pixels
      fracNPix  = float(nPixAct)/float(nPixMax)

      # the axis ratio
      axisRatioAct = float(naxes[0])/float(naxes[1])

      nPixSampled = [0,0]
      while nPixAct>nPixMax:

          if axisRatioAct >= 1.0:
              minIncr[0] += 1
          else:
              minIncr[1] += 1

          divModX = divmod(naxes[0], minIncr[0])
          print divModX
          divModY = divmod(naxes[1], minIncr[1])
          print divModY

          if divModX[1]:
              nPixSampled[0] = divModX[0]+1
          else:
              nPixSampled[0] = divModX[0]

          if divModY[1]:
              nPixSampled[1] = divModY[0]+1
          else:
              nPixSampled[1] = divModY[0]

          nPixAct = nPixSampled[0]*nPixSampled[1]

          # the axis ratio
          axisRatioAct = float(nPixSampled[0])/float(nPixSampled[1])

      return minIncr, nPixAct
  */
}

bool SE2BackgroundModeller::checkCompatibility(const boost::filesystem::path inputName, fitsfile* inputImg, const size_t* imgNaxes){
  int status=0;
  int naxis=0;
  int bitpix=0;
  long naxes[2] = {0,0};

  // get the parameters from the weight image;
  // check for the image dimension
  fits_get_img_param(inputImg, 2,  &bitpix, &naxis, naxes, &status);
  if (status){
    throw Elements::Exception() << "Problem reading parameters from image: " << inputName << "!";
    //Utils::throwElementsException(std::string("Problem reading parameters from image: ")+inputName.generic_string()+std::string("!"));
  }
  if (naxis!=2){
    throw Elements::Exception() << "The image: " << inputName << " has " << naxis << "!=2 dimensions!";
    //Utils::throwElementsException(std::string("The image: ")+inputName.generic_string()+std::string(" has ")+tostr(naxis)+std::string("!=2 dimensions!"));
  }

  // compare the dimension of the weight image
  // with the data image;
  // throw an error if they do not match
  if (imgNaxes[0] != (size_t)naxes[0]){
    throw Elements::Exception() << "The weight image: " << itsInputWeightName << " has incompatible size in x!";
    //Utils::throwElementsException(std::string("The image: ")+inputName.generic_string()+std::string(" has incompatible size in x!"));
  }
  if (imgNaxes[1] != (size_t)naxes[1]){
    throw Elements::Exception() << "The weight image: " << itsInputWeightName << " has incompatible size in y!";
    //Utils::throwElementsException(std::string("The image: ")+inputName.generic_string()+std::string(" has incompatible size in y!"));
  }
  return true;
}

bool SE2BackgroundModeller::checkCompatibility(fitsfile* inputWeight, const size_t* imgNaxes){
  int status=0;
  int naxis=0;
  int bitpix=0;
  long naxes[2] = {0,0};

  // get the parameters from the weight image;
  // check for the image dimension
  fits_get_img_param(inputWeight, 2,  &bitpix, &naxis, naxes, &status);
  if (status){
    throw Elements::Exception() << "Problem reading parameters from weight: " << itsInputWeightName << "!";
    //Utils::throwElementsException(std::string("Problem reading parameters from image: ")+itsInputWeightName.generic_string()+std::string("!"));
  }
  if (naxis!=2){
    throw Elements::Exception() << "The image: " << itsInputWeightName << " has " << naxis << "!=2 dimensions!";
    //Utils::throwElementsException(std::string("The image: ")+itsInputWeightName.generic_string()+std::string(" has ")+tostr(naxis)+std::string("!=2 dimensions!"));
  }

  // compare the dimension of the weight image
  // with the data image;
  // throw an error if they do not match
  if (imgNaxes[0] != (size_t)naxes[0]){
    throw Elements::Exception() << "The weight image: " << itsInputWeightName << " has incompatible size in x!";
    //Utils::throwElementsException(std::string("The image: ")+itsInputWeightName.generic_string()+std::string(" has incompatible size in x!"));
  }
  if (imgNaxes[1] != (size_t)naxes[1]){
    throw Elements::Exception() << "The weight image: " << itsInputWeightName << " has incompatible size in y!";
    //Utils::throwElementsException(std::string("The image: ")+itsInputWeightName.generic_string()+std::string(" has incompatible size in y!"));
  }
  return true;
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
      actRatio = bckSigVals[index] / sqrt(whtMeanVals[index]);
      if (actRatio>0.0){
        ratio[nr]=actRatio;
        nr++;
      }
    }
  }

  // use the median ratio as scaling factor
  sigFac = SplineModel::fqMedian(ratio, nr);

  // count leading 0.0 values in the list
  for (lowIndex=0; lowIndex<nr && ratio[lowIndex]<=0.0; lowIndex++);

  // re-calculated the median, omitting
  // the leading 0.0 values; I can't see
  // how there should be leading zeros
  // TODO:  check whether this make sense
  if (lowIndex>0){
    if (lowIndex<nr){
      // make a log message
      //Utils::generalLogger(std::string("Re-calculating the scaling due to leading zeros!"));
      std::cout << "Re-calculating the scaling due to leading zeros!" << std::endl;
      sigFac = SplineModel::fqMedian(ratio+lowIndex, nr-lowIndex);
    }
    else {
      //warning("Null or negative global weighting factor:","defaulted to 1");
      std::cout << "Null or negative global weighting factor: " << " | " << lowIndex << "defaulted to 1 " << nr;
      //Utils::generalLogger(std::string("Null or negative global weighting factor:") + tostr(sigFac) + std::string(" | ") + tostr(lowIndex) + std::string("defaulted to 1 ") + tostr(nr));
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
    throw Elements::Exception() << "The weight threshold is: " << weightThreshold << " but can not be smaller than 0.0!";
    //Utils::throwElementsException(std::string("The weight threshold is: ")+tostr(weightThreshold)+std::string(" but can not be smaller than 0.0!"));
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

void SE2BackgroundModeller::storeScalingFactor(PIXTYPE &sigFac)
{
  fitsfile *ioFits=NULL;
  int status=0;

  // close the READONLY FITS file
  fits_close_file(itsInputWeight, &status);
  if (status){
    throw Elements::Exception() << "Problem closing the image: " << itsInputWeightName << "!";
    //Utils::throwElementsException(std::string("Problem closing the image: ")+itsInputWeightName.generic_string()+std::string("!"));
  }

  // open the FITS file now in READWRITE mode
  fits_open_image(&ioFits, itsInputWeightName.generic_string().c_str(), READWRITE, &status);
  if (status){
    throw Elements::Exception() << "Problem closing the image: " << itsInputWeightName << "!";
    //Utils::throwElementsException(std::string("Problem opening the image: ")+itsInputWeightName.generic_string()+std::string(" in READWRITE mode!"));
  }

  // write the scaling factor to the weight image
  fits_update_key(ioFits, TFLOAT, "SIGFAC", &sigFac, "scaling factor for the weight (in variance)", &status);
  if (status){
    char fitsErrorChar[MAXCHAR];
    fits_get_errstatus(status, fitsErrorChar);
    std::cout << std::string(fitsErrorChar);
    throw Elements::Exception() << "Problems writing keyword 'SIGFAC' to the FITS file: " << itsInputWeightName << "!";
    //Utils::generalLogger(std::string(fitsErrorChar));
    //Utils::throwElementsException(std::string("Problems writing keyword 'SIGFAC' to the FITS file: ")+itsInputWeightName.generic_string()+std::string("!"));
  }

  // write down the name of the 'root' image for computing the scale factor
  fits_update_key(ioFits, TSTRING, "SIGROOT", (void*)itsInputFileName.generic_string().c_str(), "image used to compute scaling", &status);
  if (status){
    char fitsErrorChar[MAXCHAR];
    fits_get_errstatus(status, fitsErrorChar);
    std::cout << std::string(fitsErrorChar);
    throw Elements::Exception() << "Problems writing keyword 'SIGROOT' to the FITS file: " << itsInputWeightName << "!";
    //Utils::generalLogger(std::string(fitsErrorChar));
    //Utils::throwElementsException(std::string("Problems writing keyword 'SIGROOT' to the FITS file: ")+itsInputWeightName.generic_string()+std::string("!"));
  }

  // close the READWRITE image now
  fits_close_file(ioFits, &status);
  if (status){
    throw Elements::Exception() << "Problem closing the image: " << itsInputWeightName << "!";
    //Utils::throwElementsException(std::string("Problem closing the image: ")+itsInputWeightName.generic_string()+std::string("!"));
  }

  // go back to the initial state: open the image in READONLY mode, store the pointer
  fits_open_image(&itsInputWeight, itsInputWeightName.generic_string().c_str(), READONLY, &status);
  if (status){
    throw Elements::Exception() << "Problem closing the image: " << itsInputWeightName << "!";
    //Utils::throwElementsException(std::string("Problem opening the image: ")+itsInputWeightName.generic_string()+std::string(" in READONLY mode!"));
  }

  // give some feedback
  std::cout << "Scaling factor stored in the weight file!";
  //Utils::generalLogger(std::string("Scaling factor stored in the weight file!"));
}
PIXTYPE *SE2BackgroundModeller::getWhtMeanVals()
{
  return itsWhtMeanVals;
}

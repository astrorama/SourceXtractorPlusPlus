/*
 * Created on Jan 05, 2015
 * @author: mkuemmel@usm.lmu.de
 *
 * Date:     $Date$
 * Revision: $Revision$
 * Author:   $Author$
 */
#ifndef BACKGROUNDMODELLER_H
#define	BACKGROUNDMODELLER_H

#include <vector>
#include <memory>
#include <boost/filesystem.hpp>
#include "fitsio.h"
#include "SEFramework/Image/Image.h"
#include "SEImplementation/Background/BackgroundDefine.h"
#include "SEImplementation/Background/SplineModel.h"

class SE2BackgroundModeller {

public:
  SE2BackgroundModeller(std::shared_ptr<SExtractor::DetectionImage> image, std::shared_ptr<SExtractor::WeightImage> variance_map=NULL, std::shared_ptr<SExtractor::Image<unsigned char>> mask=NULL, const int weight_type_flag=0x0001);
  SE2BackgroundModeller(const boost::filesystem::path& fits_filename, const boost::filesystem::path& weight_filename,const boost::filesystem::path& mask_filename,  const int mask_type, const int weight_type_flag=0x0001);
  virtual ~SE2BackgroundModeller();

  void createModels(SplineModel **bckSpline, SplineModel **sigmaSpline, PIXTYPE &sigFac, const size_t *bckCellSize, const PIXTYPE weightThreshold,  const bool &storeScaleFactor=false);
  void createSE2Models(SplineModel **bckSpline, SplineModel **sigmaSpline, PIXTYPE &sigFac, const size_t *bckCellSize, const PIXTYPE weightThreshold,  const bool &storeScaleFactor=false);

  //
  PIXTYPE* getWhtMeanVals();
  void computeScalingFactor(PIXTYPE* whtMeanVals, PIXTYPE* bckSigVals, PIXTYPE& sigFac, const size_t nGridPoints);
  ///
private:
  void getMinIncr(size_t &nElements, long *incr, size_t *subImgNaxes);
  bool checkCompatibility(const boost::filesystem::path itsInputWeightName, fitsfile* itsInputWeight, const size_t* itsNaxes);
  bool checkCompatibility(fitsfile* itsInputWeight, const size_t* itsNaxes);

  void filter(PIXTYPE* bckVals, PIXTYPE* sigmaVals, const size_t* gridSize, const size_t* filterSize, const float &filterThreshold=0.0);
  void replaceUNDEF(PIXTYPE* bckVals, PIXTYPE* sigmaVals,const size_t* gridSize);
  void filterMedian(PIXTYPE* bckVals, PIXTYPE* sigmaVals,const size_t* gridSize, const size_t* filterSize, const float filterThresh=0.0);

  void rescaleThreshold(PIXTYPE &weightVarThreshold, const PIXTYPE &weightThreshold);
  void storeScalingFactor(PIXTYPE &sigFac);

  // input to create the object
  boost::filesystem::path itsInputMaskName;
  boost::filesystem::path itsInputFileName;
  boost::filesystem::path itsInputWeightName;
  int itsWeightTypeFlag;
  int itsMaskType;

  std::shared_ptr<SExtractor::DetectionImage> itsImage=NULL;
  std::shared_ptr<SExtractor::WeightImage> itsVariance=NULL;
  std::shared_ptr<SExtractor::Image<unsigned char>> itsMask=NULL;

  fitsfile* itsInputMask=NULL;
  fitsfile* itsInputFits=NULL;
  fitsfile* itsInputWeight=NULL;
  bool itsHasWeights=false;
  bool itsHasMask=false;
  //
  PIXTYPE* itsWhtMeanVals=NULL;
  //
  // data from the input image
  int itsBitpix=0;
  int itsNaxis=0;
  size_t itsNaxes[2]={0,0};
};

#endif	// BACKGROUNDMODELLER_H


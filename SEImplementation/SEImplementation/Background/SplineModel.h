/*
 * Created on Jan 05, 2015
 * @author: mkuemmel@usm.lmu.de
 *
 * Date:     $Date$
 * Revision: $Revision$
 * Author:   $Author$
 */
#ifndef SPLINEMODEL_H
#define	SPLINEMODEL_H

#include <boost/filesystem.hpp>
#include "SEImplementation/Background/BackgroundDefine.h"

namespace SExtractor {

class SplineModel {

public:

  SplineModel(const size_t* naxes, const size_t* gridCellSize, const size_t* nGrid, PIXTYPE* gridData);

  SplineModel(const boost::filesystem::path modelFile);
  virtual ~SplineModel();

  void gridToFits(boost::filesystem::path& fitsName, const bool overwrite=true);
  void toFits(boost::filesystem::path& fitsName, const bool overwrite=true);
  void splineLine(PIXTYPE *line, const size_t y, const size_t xStart, const size_t width);
  PIXTYPE& getMedian();
  PIXTYPE * getData();
  PIXTYPE * getDeriv();
  size_t*  getGridCellSize();
  size_t*  getNGrid();
  size_t*  getNaxes();
  size_t  getNGridPoints();
private:
  PIXTYPE* makeSplineDeriv(const size_t* nGrid, PIXTYPE* gridData);
  PIXTYPE* loadModelFromFits(const boost::filesystem::path);
  PIXTYPE  computeMedian(PIXTYPE* gridData, const size_t nGridPoints);

  size_t itsNaxes[2]={0,0};         // dimension of the image for which the spline was done
  size_t itsGridCellSize[2]={0,0};  // mesh size in x/y of the spline
  size_t itsNGrid[2]={0,0};         // number of mesh points in x/y
  size_t itsNGridPoints;

  PIXTYPE* itsGridData=NULL;
  PIXTYPE* itsDerivData=NULL;

  PIXTYPE itsMedianValue=0.0;
};

} // end of namespace SExtractor

#endif	// SPLINEMODEL_H


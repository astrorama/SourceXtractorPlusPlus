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
#ifndef SPLINEMODEL_H
#define	SPLINEMODEL_H

#include <boost/filesystem.hpp>
#include "SEImplementation/Background/SE2/BackgroundDefine.h"

namespace SourceXtractor {

class SplineModel {

public:

  SplineModel(const size_t* naxes, const size_t* gridCellSize, const size_t* nGrid, PIXTYPE* gridData);

  SplineModel(const boost::filesystem::path modelFile);
  virtual ~SplineModel();

  void gridToFits(boost::filesystem::path& fitsName, const bool overwrite=true);
  void toFits(boost::filesystem::path& fitsName, const bool overwrite=true);
  PIXTYPE  getValue(size_t x, size_t y);
  //void splineLine(PIXTYPE *line, const size_t y, const size_t xStart, const size_t width);
  PIXTYPE& getMedian();
  PIXTYPE * getData();
  PIXTYPE * getDeriv();
  size_t*  getGridCellSize();
  size_t*  getNGrid();
  size_t*  getNaxes();
  size_t  getNGridPoints();
private:
  void splineLine(PIXTYPE *line, const size_t y, const size_t xStart, const size_t width);
  PIXTYPE* makeSplineDeriv(const size_t* nGrid, PIXTYPE* gridData);
  PIXTYPE* loadModelFromFits(const boost::filesystem::path);
  PIXTYPE  computeMedian(PIXTYPE* gridData, const size_t nGridPoints);

  size_t itsNaxes[2]={0,0};         // dimension of the image for which the spline was done
  size_t itsGridCellSize[2]={0,0};  // mesh size in x/y of the spline
  size_t itsNGrid[2]={0,0};         // number of mesh points in x/y
  size_t itsNGridPoints;

  PIXTYPE* itsGridData=NULL;
  PIXTYPE* itsDerivData=NULL;

  PIXTYPE* itsBackLine=NULL;
  size_t itsBackLineY=-1;

  PIXTYPE itsMedianValue=0.0;
};

} // end of namespace SourceXtractor

#endif	// SPLINEMODEL_H


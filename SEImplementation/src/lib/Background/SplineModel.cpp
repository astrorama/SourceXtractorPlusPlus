/*
 * Created on Jan 05, 2015
 * @author: mkuemmel@usm.lmu.de
 *
 * Date:     $Date$
 * Revision: $Revision$
 * Author:   $Author$
 */

#include <math.h>
#include <boost/filesystem.hpp>             // for boost path type
#include "fitsio.h"
#include "ElementsKernel/Exception.h"       // for Elements Exception
//#include "BackgroundModule/BackgroundDefine.h"
//#include "BackgroundModule/UtilFunctions.h"
//#include "BackgroundModule/SplineModel.h"
#include "SEImplementation/Background/BackgroundDefine.h"
#include "SEImplementation/Background/SE2BackgroundUtils.h"
#include "SEImplementation/Background/SplineModel.h"
#include <iostream>
//////////
//#define	QMALLOC(ptr, typ, nel) ptr = (typ *)malloc((size_t)(nel)*sizeof(typ))

namespace SExtractor {

using namespace std;

SplineModel::SplineModel (const size_t* naxes, const size_t* gridCellSize, const size_t* nGrid, PIXTYPE* gridData) {

  itsNaxes[0] = naxes[0];
  itsNaxes[1] = naxes[1];
  itsGridCellSize[0] = gridCellSize[0];
  itsGridCellSize[1] = gridCellSize[1];
  itsNGrid[0] = nGrid[0];
  itsNGrid[1] = nGrid[1];
  itsNGridPoints = nGrid[0] * nGrid[1];

  itsGridData = gridData;
  itsDerivData = makeSplineDeriv(itsNGrid, gridData);

  itsMedianValue = computeMedian(itsGridData, itsNGridPoints);
}

SplineModel::SplineModel (const boost::filesystem::path modelFile) {
  itsGridData = loadModelFromFits(modelFile);
  itsDerivData = makeSplineDeriv(itsNGrid, itsGridData);
}

size_t* SplineModel::getGridCellSize () {
  return itsGridCellSize;
}

size_t * SplineModel::getNGrid () {
  return itsNGrid;
}

size_t SplineModel::getNGridPoints () {
  return itsNGridPoints;
}

size_t* SplineModel::getNaxes () {
  return itsNaxes;
}

PIXTYPE& SplineModel::getMedian () {
  return itsMedianValue;
}

PIXTYPE* SplineModel::getData () {
  return itsGridData;
}

SplineModel::~SplineModel () {
  // delete the grid data
  if (itsGridData)
    delete[] itsGridData;
  itsGridData = NULL;

  // delete the derivative data
  if (itsDerivData)
    delete[] itsDerivData;
  itsDerivData = NULL;
}

void SplineModel::gridToFits (boost::filesystem::path& fitsName, const bool overwrite) {
  int status = 0;
  long naxes[2] = { 0, 0 };
  long fpixel[2] = { 1, 1 };
  fitsfile *outFits = NULL;
  PIXTYPE undefNumber = -BIG;

  // delete any previous versions
  // of this file if allowed
  if (fitsName.string().size() < 1) {
    throw Elements::Exception() << "No output filename provided!";
    //Utils::throwElementsException(std::string("No output filename provided!"));
  } else {
    if (boost::filesystem::exists(fitsName)) {
      if (overwrite)
        boost::filesystem::remove(fitsName);
      else {
        throw Elements::Exception() << "The file:" << fitsName <<  " does already exist!";
        //Utils::throwElementsException(std::string("The file:") + fitsName.generic_string() + std::string(" does already exist!"));
        }
    }
  }

  // create a new FITS image
  fits_create_file(&outFits, fitsName.string().c_str(), &status);
  if (status) {
    throw Elements::Exception() << "Problems opening the new FITS file:" << fitsName <<  "!";
    //Utils::throwElementsException(std::string("Problems opening the new FITS file: ") + fitsName.generic_string() + std::string("!"));
    }

  // create an image HDU as the
  // primary extension
  naxes[0] = itsNGrid[0];
  naxes[1] = itsNGrid[1];
  fits_create_img(outFits, -32, 2, naxes, &status);
  if (status) {
    throw Elements::Exception() << "Problems creating an image extension for: " << fitsName <<  "!";
    //Utils::throwElementsException(std::string("Problems creating an image extension for: ") + fitsName.generic_string() + std::string("!"));
    }

  // fill in the pixels into the (new) image
  fits_write_pixnull(outFits, TFLOAT, fpixel, (long) itsNGrid[0] * itsNGrid[1], itsGridData, &undefNumber, &status);
  if (status) {
    fits_report_error(stderr, status);
    throw Elements::Exception() << "Problems writing data to the FITS file: " << fitsName <<  "!";
    //Utils::throwElementsException(std::string("Problems writing data to the FITS file: ") + fitsName.generic_string() + std::string("!"));
    }

  // store the metadata for the spline model in keywords:
  // NXIMG1   = itsNaxes[0] the x-dimension of the base image
  // NXIMG2   = itsNaxes[1] the y-dimension of the base image
  // BCKGRID1 = itsGridCellSize[0] the grid cell size in x
  // BCKGRID2 = itsGridCellSize[1] the grid cell size in y
  fits_write_key(outFits, TLONG, "NXIMG1", &itsNaxes[0], "x-dimension of the original image", &status);
  if (status) {
    fits_report_error(stderr, status);
    throw Elements::Exception() << "Problems writing keyword 'NXIMG1' to the FITS file: " << fitsName <<  "!";
    //Utils::throwElementsException(std::string("Problems writing keyword 'NXIMG1' to the FITS file: ") + fitsName.generic_string()+ std::string("!"));
    }
  fits_write_key(outFits, TLONG, "NXIMG2", &itsNaxes[1], "y-dimension of the original image", &status);
  if (status) {
    fits_report_error(stderr, status);
    throw Elements::Exception() << "Problems writing keyword 'NXIMG2' to the FITS file: " << fitsName <<  "!";
    //Utils::throwElementsException(std::string("Problems writing keyword 'NXIMG2' to the FITS file: ") + fitsName.generic_string()+ std::string("!"));
    }
  fits_write_key(outFits, TLONG, "BCKGRID1", &itsGridCellSize[0], "background cell size in x", &status);
  if (status) {
    fits_report_error(stderr, status);
    throw Elements::Exception() << "Problems writing keyword 'BCKGRID1' to the FITS file: " << fitsName <<  "!";
    //Utils::throwElementsException(std::string("Problems writing keyword 'BCKGRID1' to the FITS file: ") + fitsName.generic_string()+ std::string("!"));
    }
  fits_write_key(outFits, TLONG, "BCKGRID2", &itsGridCellSize[1], "background cell size in y", &status);
  if (status) {
    fits_report_error(stderr, status);
    throw Elements::Exception() << "Problems writing keyword 'BCKGRID2' to the FITS file: " << fitsName <<  "!";
    //Utils::throwElementsException(std::string("Problems writing keyword 'BCKGRID2' to the FITS file: ") + fitsName.generic_string()+ std::string("!"));
    }
  fits_write_key(outFits, TFLOAT, "MEDIAN", &itsMedianValue, "median value in grid", &status);
  if (status) {
    fits_report_error(stderr, status);
    throw Elements::Exception() << "Problems writing keyword 'MEDIAN' to the FITS file: " << fitsName <<  "!";
    //Utils::throwElementsException(std::string("Problems writing keyword 'MEDIAN' to the FITS file: ") + fitsName.generic_string()+ std::string("!"));
    }

  // close the FITS file
  fits_close_file(outFits, &status);
  if (status) {
    throw Elements::Exception() << "Problems closing the FITS file: " << fitsName <<  "!";
    //Utils::throwElementsException(std::string("Problems closing the FITS file: ") + fitsName.generic_string() + std::string("!"));
    }
}

void SplineModel::toFits (boost::filesystem::path& fitsName, const bool overwrite) {
  int status = 0;
  long naxes[2] = { 0, 0 };
  long fpixel[2] = { 1, 1 };
  fitsfile *outFits = NULL;
  PIXTYPE undefNumber = -BIG;
  PIXTYPE* pixBuffer = NULL;

  // delete any previous versions
  // of this file if allowed
  if (fitsName.string().size() < 1) {
    throw Elements::Exception() << "No output filename provided!";
    //Utils::throwElementsException(std::string("No output filename provided!"));
  } else {
    if (boost::filesystem::exists(fitsName)) {
      if (overwrite)
        boost::filesystem::remove(fitsName);
      else {
        throw Elements::Exception() << "The file:" << fitsName <<  " does already exist!";
        //Utils::throwElementsException(
        //    std::string("The file:") + fitsName.generic_string() + std::string(" does already exist!"));
      }
    }
  }

  // create a new FITS image
  fits_create_file(&outFits, fitsName.string().c_str(), &status);
  if (status) {
    throw Elements::Exception() << "Problems opening the new FITS file:" << fitsName <<  "!";
    //Utils::throwElementsException(
    //    std::string("Problems opening the new FITS file:") + fitsName.generic_string() + std::string("!"));
  }

  // create an image HDU as the
  // primary extension
  naxes[0] = itsNaxes[0];
  naxes[1] = itsNaxes[1];
  fits_create_img(outFits, -32, 2, naxes, &status);
  if (status) {
    throw Elements::Exception() << "Problems creating an image extension for: " << fitsName <<  "!";
    //Utils::throwElementsException(
    //    std::string("Problems creating an image extension for: ") + fitsName.generic_string() + std::string("!"));
  }

  // allocate memory for the buffer
  pixBuffer = new PIXTYPE[itsNaxes[0]];

  // go over all rows
  for (size_t yIndex = 0; yIndex < itsNaxes[1]; yIndex++) {
    // evaluate the spline for one row
    splineLine(pixBuffer, yIndex, 0, itsNaxes[0]);

    // set the new starting position
    // at the beginning of the row
    fpixel[1] = (long) yIndex + 1;

    // fill in the pixels of the row into the (new) image
    fits_write_pixnull(outFits, TFLOAT, fpixel, (long) itsNaxes[0], pixBuffer, &undefNumber, &status);
    if (status) {
      fits_report_error(stderr, status);
      throw Elements::Exception() << "Problems writing data to the FITS file: " << fitsName <<  "!";
      //Utils::throwElementsException(
      //    std::string("Problems writing data to the FITS file: ") + fitsName.generic_string() + std::string("!"));
    }
  }

  // close the FITS file
  fits_close_file(outFits, &status);
  if (status) {
    throw Elements::Exception() << "Problems closing the FITS file: " << fitsName <<  "!";
    //Utils::throwElementsException(
    //    std::string("Problems writing data to the FITS file: ") + fitsName.generic_string() + std::string("!"));
  }

  // release the memory
  if (pixBuffer)
    delete[] pixBuffer;
}

void SplineModel::splineLine (PIXTYPE *line, const size_t y, const size_t xStart, const size_t width) {
  int i, j, x, yl, nbx, nbxm1, nby, nx, ystep, changepoint;
  float dx, dx0, dy, dy3, cdx, cdy, cdy3, temp, xstep, *node, *nodep, *dnode, *blo, *bhi, *dblo, *dbhi, *u;
  PIXTYPE *backline;

  backline = line;
  //Utils::generalLogger(std::string("working on row:") + tostr(y));

  nbx = itsNGrid[0];
  nbxm1 = nbx - 1;
  nby = itsNGrid[1];

  if (nby > 1) {
    dy = (float) y / itsGridCellSize[1] - 0.5;
    dy -= (yl = (int) dy);
    if (yl < 0) {
      yl = 0;
      dy -= 1.0;
    } else if (yl >= nby - 1) {
      yl = nby < 2 ? 0 : nby - 2;
      dy += 1.0;
    }

    /*-- Interpolation along y for each node */
    cdy = 1 - dy;
    dy3 = (dy * dy * dy - dy);
    cdy3 = (cdy * cdy * cdy - cdy);
    ystep = nbx * yl;
    blo = itsGridData + ystep;
    bhi = blo + nbx;
    dblo = itsDerivData + ystep;
    dbhi = dblo + nbx;
    //QMALLOC(node, float, nbx);  /* Interpolated background */
    node = new float[nbx]; /* Interpolated background */
    nodep = node;
    for (x = nbx; x--;)
      *(nodep++) = cdy * *(blo++) + dy * *(bhi++) + cdy3 * *(dblo++) + dy3 * *(dbhi++);

    /*-- Computation of 2nd derivatives along x */
    //QMALLOC(dnode, float, nbx); /* 2nd derivative along x */
    dnode = new float[nbx]; /* 2nd derivative along x */
    if (nbx > 1) {
      //QMALLOC(u, float, nbxm1); /* temporary array */
      u = new float[nbxm1]; /* temporary array */
      *dnode = *u = 0.0; /* "natural" lower boundary condition */
      nodep = node + 1;
      for (x = nbxm1; --x; nodep++) {
        temp = -1 / (*(dnode++) + 4);
        *dnode = temp;
        temp *= *(u++) - 6 * (*(nodep + 1) + *(nodep - 1) - 2 * *nodep);
        *u = temp;
      }
      *(++dnode) = 0.0; /* "natural" upper boundary condition */
      for (x = nbx - 2; x--;) {
        temp = *(dnode--);
        *dnode = (*dnode * temp + *(u--)) / 6.0;
      }
      delete[] u;
      dnode--;
    }
  } else {
    /*-- No interpolation and no new 2nd derivatives needed along y */
    node = itsGridData;
    dnode = itsDerivData;
  }

  /*-- Interpolation along x */
  if (nbx > 1) {
    nx = itsGridCellSize[0];
    xstep = 1.0 / nx;
    changepoint = nx / 2;
    dx = (xstep - 1) / 2; /* dx of the first pixel in the row */
    dx0 = ((nx + 1) % 2) * xstep / 2; /* dx of the 1st pixel right to a bkgnd node */
    blo = node;
    bhi = node + 1;
    dblo = dnode;
    dbhi = dnode + 1;
    for (x = i = 0, j = xStart; j--; i++, dx += xstep) {
      if (i == changepoint && x > 0 && x < nbxm1) {
        blo++;
        bhi++;
        dblo++;
        dbhi++;
        dx = dx0;
      }
      cdx = 1 - dx;
      if (i == nx) {
        x++;
        i = 0;
      }
    }

    for (j = width; j--; i++, dx += xstep) {
      if (i == changepoint && x > 0 && x < nbxm1) {
        blo++;
        bhi++;
        dblo++;
        dbhi++;
        dx = dx0;
      }
      cdx = 1 - dx;

      //*(line++) -= (*(backline++) = (PIXTYPE)(cdx*(*blo+(cdx*cdx-1)**dblo) + dx*(*bhi+(dx*dx-1)**dbhi)));
      *(backline++) = (PIXTYPE) (cdx * (*blo + (cdx * cdx - 1) * *dblo) + dx * (*bhi + (dx * dx - 1) * *dbhi));
      if (i == nx) {
        x++;
        i = 0;
      }
    }
  } else
    for (j = width; j--;)
      //*(line++) -= (*(backline++) = (PIXTYPE)*node);
      *(backline++) = (PIXTYPE) *node;

  if (nby > 1) {
    delete[] node;
    delete[] dnode;
  }

  return;
}

PIXTYPE* SplineModel::makeSplineDeriv (const size_t* nGrid, PIXTYPE* gridData) {
  int x, y, nbx, nby, nbym1;
  PIXTYPE *dmap, *dmapt, *mapt, *u, temp;
  //float *dmap,*dmapt,*mapt, *u, temp;
  nbx = nGrid[0];
  nby = nGrid[1];
  nbym1 = nby - 1;
  //QMALLOC(dmap, float, nGrid[0]*nGrid[1]);

  dmap = new PIXTYPE[nGrid[0] * nGrid[1]];

  for (x = 0; x < nbx; x++) {
    mapt = gridData + x;
    dmapt = dmap + x;

    if (nby > 1) {
      //QMALLOC(u, float, nbym1); // temporary array
      u = new PIXTYPE[nbym1];       // temporary array
      *dmapt = *u = 0.0;          // "natural" lower boundary condition

      mapt += nbx;

      for (y = 1; y < nbym1; y++, mapt += nbx) {
        temp = -1 / (*dmapt + 4);

        *(dmapt += nbx) = temp;
        temp *= *(u++) - 6 * (*(mapt + nbx) + *(mapt - nbx) - 2 * *mapt);

        *u = temp;
      }
      *(dmapt += nbx) = 0.0; /* "natural" upper boundary condition */
      for (y = nby - 2; y--;) {
        temp = *dmapt;
        dmapt -= nbx;
        *dmapt = (*dmapt * temp + *(u--)) / 6.0;
      }
      //free(u);
      delete[] u;
    } else
      *dmapt = 0.0;
  }
  return dmap;
}

PIXTYPE* SplineModel::loadModelFromFits (const boost::filesystem::path modelFile) {
  int status = 0;
  int bitpix;
  int naxis;
  int anynul = 0;
  long naxes[2];
  long fpixel[2] = { 1, 1 };
  char someComment[80];

  size_t gridSize[2] = { 0, 0 };
  ldiv_t divResult;

  PIXTYPE* gridData;
  PIXTYPE undefNumber = -BIG;

  fitsfile* inputFits = NULL;

  // open the FITS file and store the pointer; give some logger info
  fits_open_image(&inputFits, modelFile.string().c_str(), READONLY, &status);
  if (status) {
    throw Elements::Exception() << "Problem opening the image: " << modelFile << "!";
    //Utils::throwElementsException(
    //    std::string("Problem opening the image: ") + modelFile.generic_string() + std::string("!"));
  }

  // get the basic image information;
  // make sure the image has two dimensions
  fits_get_img_param(inputFits, 2, &bitpix, &naxis, naxes, &status);
  if (status) {
    throw Elements::Exception() << "Problem reading parameters from image: " << modelFile << "!";
    //Utils::throwElementsException(
    //    std::string("Problem reading parameters from image: ") + modelFile.generic_string() + std::string("!"));
  }
  if (naxis != 2) {
    throw Elements::Exception() << "The image: " << modelFile << " has " << naxis <<"!=2 dimensions!";
    //Utils::throwElementsException(
    //    std::string("The image: ") + modelFile.generic_string() + std::string(" has ") + tostr(naxis)
    //        + std::string("!=2 dimensions!"));
  }
  itsNGrid[0] = (size_t) naxes[0];
  itsNGrid[1] = (size_t) naxes[1];

  // read in the keywords NXIMG1 and NXIMG2
  fits_read_key(inputFits, TLONG, "NXIMG1", &itsNaxes[0], someComment, &status);
  if (status) {
    if (status == KEY_NO_EXIST || status == VALUE_UNDEFINED) {
      throw Elements::Exception() << "Keyword 'NXIMG1' does not exist in image: " << modelFile << "!";
      //Utils::throwElementsException(
      //    std::string("Keyword 'NXIMG1' does not exist in image: ") + modelFile.generic_string() + std::string("!"));
    }
    throw Elements::Exception() << "Problem reading keyword 'NXIMG1' from image: " << modelFile << "!";
    //Utils::throwElementsException(
    //    std::string("Problem reading keyword 'NXIMG1' from image: ") + modelFile.generic_string() + std::string("!"));
  }
  fits_read_key(inputFits, TLONG, "NXIMG2", &itsNaxes[1], someComment, &status);
  if (status) {
    if (status == KEY_NO_EXIST || status == VALUE_UNDEFINED) {
      throw Elements::Exception() << "Keyword 'NXIMG2' does not exist in image: " << modelFile << "!";
      //Utils::throwElementsException(
      //    std::string("Keyword 'NXIMG2' does not exist in image: ") + modelFile.generic_string() + std::string("!"));
    }
    throw Elements::Exception() << "Problem reading keyword 'NXIMG2' from image: " << modelFile << "!";
    //Utils::throwElementsException(
    //    std::string("Problem reading keyword 'NXIMG2' from image: ") + modelFile.generic_string() + std::string("!"));
  }

  // read in the keywords BCKGRID1 and BCKGRID2
  fits_read_key(inputFits, TLONG, "BCKGRID1", &itsGridCellSize[0], someComment, &status);
  if (status) {
    if (status == KEY_NO_EXIST || status == VALUE_UNDEFINED) {
      throw Elements::Exception() << "Keyword 'BCKGRID1' does not exist in image: " << modelFile << "!";
      //Utils::throwElementsException(
      //    std::string("Keyword 'BCKGRID1' does not exist in image: ") + modelFile.generic_string() + std::string("!"));
    }
    throw Elements::Exception() << "Problem reading keyword 'BCKGRID1' from image: " << modelFile << "!";
    //Utils::throwElementsException(
    //    std::string("Problem reading keyword 'BCKGRID1' from image: ") + modelFile.generic_string() + std::string("!"));
  }
  fits_read_key(inputFits, TLONG, "BCKGRID2", &itsGridCellSize[1], someComment, &status);
  if (status) {
    if (status == KEY_NO_EXIST || status == VALUE_UNDEFINED) {
      throw Elements::Exception() << "Keyword 'BCKGRID2' does not exist in image: " << modelFile << "!";
      //Utils::throwElementsException(
      //    std::string("Keyword 'BCKGRID2' does not exist in image: ") + modelFile.generic_string() + std::string("!"));
    }
    throw Elements::Exception() << "Problem reading keyword 'BCKGRID2' from image: " << modelFile << "!";
    //Utils::throwElementsException(
    //    std::string("Problem reading keyword 'BCKGRID2' from image: ") + modelFile.generic_string() + std::string("!"));
  }

  // read in the keyword 'MEDIAN'
  fits_read_key(inputFits, TFLOAT, "MEDIAN", &itsMedianValue, someComment, &status);
  if (status) {
    if (status == KEY_NO_EXIST || status == VALUE_UNDEFINED) {
      throw Elements::Exception() << "Keyword 'MEDIAN' does not exist in image: " << modelFile << "!";
      //Utils::throwElementsException(
      //    std::string("Keyword 'MEDIAN' does not exist in image: ") + modelFile.generic_string() + std::string("!"));
    }
    throw Elements::Exception() << "Problem reading keyword 'MEDIAN' from image: " << modelFile << "!";
    //Utils::throwElementsException(
    //    std::string("Problem reading keyword 'MEDIAN' from image: ") + modelFile.generic_string() + std::string("!"));
  }

  // make sure that the model x-dimension and the image dimension are identical
  divResult = std::div(long(itsNaxes[0]), long(itsGridCellSize[0]));
  gridSize[0] = size_t(divResult.quot);
  if (divResult.rem)
    gridSize[0] += 1;
  if (gridSize[0] != itsNGrid[0]) {
    throw Elements::Exception() << "Inferred and actual x-size differ in : " << modelFile << "!";
    //Utils::throwElementsException(
    //    std::string("Inferred and actual x-size differ in : ") + modelFile.generic_string() + std::string("!"));
  }
  // make sure that the model y-dimension and the image dimension are identical
  divResult = std::div(long(itsNaxes[1]), long(itsGridCellSize[1]));
  gridSize[1] = size_t(divResult.quot);
  if (divResult.rem)
    gridSize[1] += 1;
  if (gridSize[1] != itsNGrid[1]) {
    throw Elements::Exception() << "Inferred and actual y-size differ in " << modelFile << "!";
    //Utils::throwElementsException(
    //    std::string("Inferred and actual y-size differ in : ") + modelFile.generic_string() + std::string("!"));
  }
  itsNGridPoints = itsNGrid[0] * itsNGrid[1];

  // make sure not too much R=memory is deployed
  if (itsNGrid[0] * itsNGrid[1] > BACK_BUFSIZE) {
    throw Elements::Exception() << "The model in: " << modelFile << " is too large with " << itsNGrid[0]*itsNGrid[1] << " pixels!";
    //Utils::throwElementsException(
    //    std::string("The model in: ") + modelFile.generic_string() + std::string(" is too large with ")
    //        + tostr(itsNGridPoints) + std::string("pixels!"));
  }

  // allocate memory for the data
  gridData = new PIXTYPE[itsNGridPoints];

  // read all the pixels in;
  // make sure there are no undefined values
  fits_read_pix(inputFits, TFLOAT, fpixel, itsNGrid[0] * itsNGrid[1], &undefNumber, gridData, &anynul, &status);
  if (status) {
    throw Elements::Exception() << "Problem reading pixel data from image: " << modelFile << "!";
    //Utils::throwElementsException(
    //    std::string("Problem reading pixel data from image: ") + modelFile.generic_string() + std::string("!"));
  }
  if (anynul) {

    throw Elements::Exception() << "There are undefined values in the model: " << modelFile << "!";
    //Utils::throwElementsException(
    //    std::string("There are undefined values in the model: ") + modelFile.generic_string() + std::string("!"));
  }

  // close the FITS file
  fits_close_file(inputFits, &status);
  if (status) {
    throw Elements::Exception() << "Problem closing the image: " << modelFile << "!";
    //Utils::throwElementsException(
    //    std::string("Problem closing the image: ") + modelFile.generic_string() + std::string("!"));
  }
  inputFits = NULL;

  return gridData;
}

PIXTYPE SplineModel::computeMedian (PIXTYPE* itsGridData, const size_t nGridPoints) {
  PIXTYPE median;
  PIXTYPE* tmpArray;

  // create a tmp array and transfer the data
  tmpArray = new PIXTYPE[nGridPoints];
  for (size_t index = 0; index < nGridPoints; index++) {
    tmpArray[index] = itsGridData[index];
  }

  // compute the median
  median = SE2BackgroundUtils::fqMedian(tmpArray, nGridPoints);

  // the median can not be NaN, throw an Exception
  if (::isnan((double) median)) {
    throw Elements::Exception() << "The median of the background is NaN. This can not be!";
    //Utils::throwElementsException(std::string("The median of the background is NaN. This can not be!"));
  }

  // mop up and return
  delete[] tmpArray;
  return median;
}

} // end of namespace SExtractor

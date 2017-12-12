/*
 * Created on Jan 05, 2015
 * @author: mkuemmel@usm.lmu.de
 *
 * Date:     $Date$
 * Revision: $Revision$
 * Author:   $Author$
 */
#ifndef BACKGROUNDHISTOGRAM_H
#define	BACKGROUNDHISTOGRAM_H

#include <cstddef>

namespace SExtractor {

class BackgroundHistogram {

public:
  BackgroundHistogram(const double& mean, const double& sigm, const size_t& ndata);
  virtual ~BackgroundHistogram();

  //void fillInData(const PIXTYPE* cellData, const size_t ndata);
  void addDatum(const PIXTYPE& pixVal);
  
  void getBackGuessMod(PIXTYPE& bckVal, PIXTYPE& sigmaVal);
  void getBackGuess(PIXTYPE& bckVal, PIXTYPE& sigmaVal);

  double itsMean=0.0;
  double itsSigma=0.0;

private:
  size_t itsStatNData=0;

  float itsQzero  = 0.0;
  float itsQscale = 0.0;
  float itsCste   = 0.0;

  int* itsHisto=NULL;
  size_t itsNLevels=0;
};

} // end of namespace SExtractor


#endif	/* BACKGROUNDHISTOGRAM_H */


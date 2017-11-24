/*
 * Created on Jan 05, 2015
 * @author: mkuemmel@usm.lmu.de
 * Date:     $Date$
 * Revision: $Revision$
 * Author:   $Author$
 */
#ifndef SE2BACKGROUNDUTILS_H
#define	SE2BACKGROUNDUTILS_H

#include <sstream>

namespace SExtractor {

class SE2BackgroundUtils {

 public:
  //SE2BackgroundUtils(){};
  static int     fqCmp(const void *p1, const void *p2)  {
    PIXTYPE f1=*((float *)p1);
    PIXTYPE f2=*((float *)p2);
    return f1>f2? 1 : (f1<f2? -1 : 0);
  };

  static PIXTYPE fqMedian(PIXTYPE *ra, size_t n){
    qsort(ra, n, sizeof(PIXTYPE), fqCmp);
    if (n<2)
      return *ra;
    else
      return n&1? ra[n/2] : (ra[n/2-1]+ra[n/2])/2.0;
  };
  //static void    weightToVar(PIXTYPE *data, const size_t  npix, const int wTypeFlag, const PIXTYPE sigFac=1.0);
  //static void    generalLogger(std::string logString);
  //static void    throwElementsException(std::string exceptionString);
};

// general conversion from primitive types to string;
// mainly to avoid the C++11 featire "std::to_string()"
//template <typename T> std::string tostr(const T& t) {
//   std::ostringstream os;
//   os<<t;
//   return os.str();
//}
} // end of namespace SExtractor
#endif	// SE2BACKGROUNDUTILS_H


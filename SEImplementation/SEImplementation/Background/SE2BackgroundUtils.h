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
#ifndef SE2BACKGROUNDUTILS_H
#define	SE2BACKGROUNDUTILS_H

#include <sstream>
#include "ElementsKernel/Logging.h"         // for Logging::LogMessageStream, etc
#include "SEImplementation/Background/BackgroundDefine.h"

namespace SExtractor {

static Elements::Logging bck_model_logger = Elements::Logging::getLogger("BackgroundModel");

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


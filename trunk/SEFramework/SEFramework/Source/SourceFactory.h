/*
 * SourceFactory.h
 *
 *  Created on: Aug 8, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_SOURCE_SOURCEFACTORY_H_
#define _SEFRAMEWORK_SOURCE_SOURCEFACTORY_H_

#include <memory>

namespace SExtractor {

class SourceInterface;

/**
 * @class SourceFactory
 * @brief A factory interface to create SourceInterface instances
 *
 */


class SourceFactory {
public:
  virtual ~SourceFactory() = default;

  virtual std::shared_ptr<SourceInterface> createSource() const = 0;
};

}


#endif /* _SEFRAMEWORK_SOURCE_SOURCEFACTORY_H_ */

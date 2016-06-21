/*
 * AutoRegisterer.h
 *
 *  Created on: Jun 17, 2016
 *      Author: mschefer
 */

#ifndef SEFRAMEWORK_SEFRAMEWORK_REGISTRATION_AUTOREGISTERER_H_
#define SEFRAMEWORK_SEFRAMEWORK_REGISTRATION_AUTOREGISTERER_H_

#include "SEFramework/Registration/RegistrationManager.h"

namespace SExtractor {

/**
 * @class AutoRegisterer
 * @brief A template used to automatically register any type of object with the RegistrationManager
 *
 */

template<typename T, class R = RegistrationManager>
class AutoRegisterer {
public:
  AutoRegisterer() {
    R::instance().registerObject(std::move(std::unique_ptr<T>(new T)));
  }
};

} // SExtractor namespace

#endif /* SEFRAMEWORK_SEFRAMEWORK_REGISTRATION_AUTOREGISTERER_H_ */

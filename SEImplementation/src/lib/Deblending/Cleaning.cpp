/*
 * Cleaning.cpp
 *
 *  Created on: 2018 M12 18
 *      Author: mschefer
 */


#include "SEImplementation/Deblending/Cleaning.h"

namespace SExtractor {

void Cleaning::deblend(SourceGroupInterface& group) const {
  for (auto it = group.begin(); it != group.end(); ++it) {
    if (shouldClean(*it, group)) {
      group.removeSource(it);
      continue;
    }
  }
}

bool Cleaning::shouldClean(SourceInterface& source, SourceGroupInterface& group) const {
  return false;
}

}


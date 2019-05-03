/*
 * Sorter.h
 *
 *  Created on: Sep 10, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEMAIN_SORTER_H_
#define _SEMAIN_SORTER_H_

#include "SEUtils/Observable.h"
#include "SEFramework/Source/SourceGroupInterface.h"

namespace SExtractor {

class Sorter: public Observer<std::shared_ptr<SourceGroupInterface>>,
              public Observable<std::shared_ptr<SourceGroupInterface>> {
public:

  Sorter();
  virtual ~Sorter() = default;

  void handleMessage(const std::shared_ptr<SourceGroupInterface>& message) override;

private:
  std::map<int, std::shared_ptr<SourceGroupInterface>> m_output_buffer;
  int m_output_next;
};

} // end SExtractor

#endif // _SEMAIN_SORTER_H_

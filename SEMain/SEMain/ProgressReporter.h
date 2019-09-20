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
 * ProgressReporter.h
 *
 *  Created on: Mar 19, 2019
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEMAIN_PROGRESSPRINTER_H
#define _SEMAIN_PROGRESSPRINTER_H

#include <map>
#include <string>
#include "SEUtils/Observable.h"
#include "SEMain/ProgressMediator.h"

namespace SExtractor {

typedef Observer<std::list<ProgressInfo>> ProgressObserver;
typedef Observer<bool> DoneObserver;

/**
 * @class ProgressReporter
 * This interface is implemented by the concrete progress reporters, so at run time
 * the software can switch the way of informing the user about the progress.
 * Details of what is reported is abstracted via key/value.
 */
class ProgressReporter: public ProgressObserver, public DoneObserver {
public:

  virtual ~ProgressReporter() = default;

  ProgressReporter() = default;

  /**
   * To be called when there are updates on the progress
   * @param info
   *    A dictionary, where the key is the task or subprocess name (i.e. segmentation),
   *    and the value a pair, where the first is the number of entities processed, and
   *    the second the total, or -1 if unknown.
   */
  using ProgressObserver::handleMessage;

  /**
   * To be called when the full processing is done.
   */
  using DoneObserver::handleMessage;
};

} // end SExtractor

#endif // _SEMAIN_PROGRESSPRINTER_H

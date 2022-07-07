/** Copyright © 2019-2022 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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
 * Sorter.h
 *
 *  Created on: Sep 10, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEMAIN_SORTER_H_
#define _SEMAIN_SORTER_H_

#include "SEFramework/Pipeline/PipelineStage.h"
#include "SEFramework/Source/SourceGroupInterface.h"

namespace SourceXtractor {

class Sorter: public PipelineReceiver<SourceGroupInterface>, public PipelineEmitter<SourceGroupInterface> {
public:

  Sorter();
  virtual ~Sorter() = default;

  void receiveSource(std::unique_ptr<SourceGroupInterface> source) override;
  void receiveProcessSignal(const ProcessSourcesEvent& event) override;

private:
  std::map<int, std::unique_ptr<SourceGroupInterface>> m_output_buffer;
  int m_output_next;
};

} // end SourceXtractor

#endif // _SEMAIN_SORTER_H_

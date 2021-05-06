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
 * Multithreadedmeasurement->h
 *
 *  Created on: May 17, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_OUTPUT_MULTITHREADEDMEASUREMENT_H_
#define _SEIMPLEMENTATION_OUTPUT_MULTITHREADEDMEASUREMENT_H_

#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <AlexandriaKernel/ThreadPool.h>
#include "SEFramework/Pipeline/Measurement.h"

namespace SourceXtractor {

class MultithreadedMeasurement : public Measurement {
public:

  using SourceToRowConverter = std::function<Euclid::Table::Row(const SourceInterface&)>;
  MultithreadedMeasurement(SourceToRowConverter source_to_row, const std::shared_ptr<Euclid::ThreadPool>& thread_pool)
      : m_source_to_row(source_to_row),
        m_thread_pool(thread_pool),
        m_group_counter(0),
        m_input_done(false), m_abort_raised(false) {}

  virtual ~MultithreadedMeasurement();

  void handleMessage(const std::shared_ptr<SourceGroupInterface>& source_group) override;

  void startThreads() override;
  void waitForThreads() override;

private:
  static void outputThreadStatic(MultithreadedMeasurement* measurement);
  void outputThreadLoop();

  SourceToRowConverter m_source_to_row;
  std::shared_ptr<Euclid::ThreadPool> m_thread_pool;
  std::unique_ptr<std::thread> m_output_thread;

  int m_group_counter;
  std::atomic_bool m_input_done, m_abort_raised;

  std::condition_variable m_new_output;
  std::list<std::pair<int, std::shared_ptr<SourceGroupInterface>>> m_output_queue;
  std::mutex m_output_queue_mutex;
};

}

#endif /* _SEIMPLEMENTATION_OUTPUT_MULTITHREADEDMEASUREMENT_H_ */
